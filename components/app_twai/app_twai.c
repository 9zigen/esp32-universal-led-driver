#include <sys/cdefs.h>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/twai.h"

#include "board.h"
#include "app_twai.h"

#define PING_PERIOD_MS          250
#define NO_OF_DATA_MSGS         50
#define NO_OF_ITERS             3
#define ITER_DELAY_MS           1000
#define RX_TASK_PRIO            8
#define TX_TASK_PRIO            9
#define CTRL_TSK_PRIO           10
#define TX_GPIO_NUM             CAN_TX
#define RX_GPIO_NUM             CAN_RX


static const char *TAG =  "APP TWAI";
static QueueHandle_t tx_task_queue;
static QueueHandle_t rx_task_queue;
static SemaphoreHandle_t stop_ping_sem;
static SemaphoreHandle_t ctrl_task_sem;
static SemaphoreHandle_t done_sem;
static uint8_t master_node = 0;
static twai_callback_t *rx_callback = NULL;

typedef enum {
    TX_SEND_PINGS,
    TX_SEND_START_CMD,
    TX_SEND_STOP_CMD,
    TX_TASK_EXIT,
} tx_task_action_t;

typedef enum {
    RX_RECEIVE_PING_RESP,
    RX_RECEIVE_DATA,
    RX_RECEIVE_STOP_RESP,
    RX_TASK_EXIT,
} rx_task_action_t;

_Noreturn static void twai_receive_task(void *arg)
{
    twai_message_t rx_msg;
    while (1)
    {
        twai_receive(&rx_msg, portMAX_DELAY);
        if (rx_msg.identifier >= APP_TWAI_LED_BRIGHTNESS && rx_msg.identifier < APP_TWAI_LED_MAX)
        {
            uint32_t data = 0;
            data |= (rx_msg.data[0] << 24);
            data |= (rx_msg.data[1] << 16);
            data |= (rx_msg.data[2] << 8);
            data |= rx_msg.data[3];

            ESP_LOGI(TAG, "Received data value %"PRIu32" %f", data, (double)data/1000.0);

            if (rx_callback != NULL) {
                rx_callback(rx_msg.identifier, data);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
    vTaskDelete(NULL);
}

_Noreturn static void twai_transmit_task(void *arg)
{
    while (1) {
        twai_message_t msg;
        xQueueReceive(tx_task_queue, &msg, portMAX_DELAY);

        /* Queue message for transmission */
        if (twai_transmit(&msg, pdMS_TO_TICKS(1000)) == ESP_OK) {
            ESP_LOGI(TAG, "Message queued for transmission");
        } else {
            ESP_LOGE(TAG, "Failed to queue message for transmission");
        }
    }
}

static void twai_control_task(void *arg)
{
    xSemaphoreTake(ctrl_task_sem, portMAX_DELAY);
    tx_task_action_t tx_action;
    rx_task_action_t rx_action;

    for (int iter = 0; iter < NO_OF_ITERS; iter++) {
        ESP_ERROR_CHECK(twai_start());
        ESP_LOGI(TAG, "Driver started");

        //Start transmitting pings, and listen for ping response
        tx_action = TX_SEND_PINGS;
        rx_action = RX_RECEIVE_PING_RESP;
        xQueueSend(tx_task_queue, &tx_action, portMAX_DELAY);
        xQueueSend(rx_task_queue, &rx_action, portMAX_DELAY);

        //Send Start command to slave, and receive data messages
        xSemaphoreTake(ctrl_task_sem, portMAX_DELAY);
        tx_action = TX_SEND_START_CMD;
        rx_action = RX_RECEIVE_DATA;
        xQueueSend(tx_task_queue, &tx_action, portMAX_DELAY);
        xQueueSend(rx_task_queue, &rx_action, portMAX_DELAY);

        //Send Stop command to slave when enough data messages have been received. Wait for stop response
        xSemaphoreTake(ctrl_task_sem, portMAX_DELAY);
        tx_action = TX_SEND_STOP_CMD;
        rx_action = RX_RECEIVE_STOP_RESP;
        xQueueSend(tx_task_queue, &tx_action, portMAX_DELAY);
        xQueueSend(rx_task_queue, &rx_action, portMAX_DELAY);

        xSemaphoreTake(ctrl_task_sem, portMAX_DELAY);
        ESP_ERROR_CHECK(twai_stop());
        ESP_LOGI(TAG, "Driver stopped");
        vTaskDelay(pdMS_TO_TICKS(ITER_DELAY_MS));
    }
    //Stop TX and RX tasks
    tx_action = TX_TASK_EXIT;
    rx_action = RX_TASK_EXIT;
    xQueueSend(tx_task_queue, &tx_action, portMAX_DELAY);
    xQueueSend(rx_task_queue, &rx_action, portMAX_DELAY);

    //Delete Control task
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

void app_twai_send(app_twai_message_t *message)
{
    if (master_node)
    {
        twai_message_t can_msg;
        can_msg.identifier = message->id;
        can_msg.extd = 1;
        can_msg.data_length_code = message->length;

        printf("App TWAI Send: ");
        for (int i = 0; i < message->length; i++) {
            can_msg.data[i] = message->data[i];
            printf("0x%02X ", can_msg.data[i]);
        }
        printf("\n");

        xQueueSend(tx_task_queue, &can_msg, pdMS_TO_TICKS(100));
    }
}

void app_twai_register_callback(twai_callback_t * cb)
{
    if (cb != NULL) {
        rx_callback = cb;
    }
}

void app_twai_init(uint8_t is_master)
{
    //Create tasks, queues, and semaphores
    rx_task_queue = xQueueCreate(1, sizeof(rx_task_action_t));
    tx_task_queue = xQueueCreate(32, sizeof(twai_message_t));
    ctrl_task_sem = xSemaphoreCreateBinary();
    stop_ping_sem = xSemaphoreCreateBinary();
    done_sem = xSemaphoreCreateBinary();

    master_node = is_master;
    if (master_node)
    {
        xTaskCreate(twai_transmit_task, "TWAI_tx", 4096, NULL, TX_TASK_PRIO, NULL);
    }
    else
    {
        xTaskCreate(twai_receive_task, "TWAI_rx", 4096, NULL, RX_TASK_PRIO, NULL);
    }

    //Install TWAI driver
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_50KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI(TAG, "Driver installed");

    //Start TWAI driver
    if (twai_start() == ESP_OK) {
        ESP_LOGI(TAG, "Driver started");
    } else {
        ESP_LOGE(TAG, "Failed to start driver");
    }


//
//    for (;;) {
//        //Wait for message to be received
//        twai_message_t rx_message;
//        if (twai_receive(&rx_message, pdMS_TO_TICKS(10000)) == ESP_OK) {
//            printf("Message received\n");
//        } else {
//            printf("Failed to receive message\n");
//            continue;
//        }
//
//        //Process received message
//        if (rx_message.extd) {
//            printf("Message is in Extended Format\n");
//        } else {
//            printf("Message is in Standard Format\n");
//        }
//        printf("ID is %lu\n", rx_message.identifier);
//        if (!(rx_message.rtr)) {
//            for (int i = 0; i < rx_message.data_length_code; i++) {
//                printf("Data byte %d = %d\n", i, rx_message.data[i]);
//            }
//        }
//    }

//    xSemaphoreGive(ctrl_task_sem);              //Start control task
//    xSemaphoreTake(done_sem, portMAX_DELAY);    //Wait for completion
}

void app_twai_uninit()
{
    //Uninstall TWAI driver
    ESP_ERROR_CHECK(twai_driver_uninstall());
    ESP_LOGI(TAG, "Driver uninstalled");

    //Cleanup
    vQueueDelete(rx_task_queue);
    vQueueDelete(tx_task_queue);
    vSemaphoreDelete(ctrl_task_sem);
    vSemaphoreDelete(stop_ping_sem);
    vSemaphoreDelete(done_sem);
}