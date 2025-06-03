#include <sys/cdefs.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "light.h"
#include "can.h"
#include "app_twai.h"

static const char *TAG =  "APP TWAI";
static QueueHandle_t queue_can;

void can_sync_light(const double * target_duty, double target_brightness)
{
    x_light_message_t txMessage;
    for (uint8_t i = 0; i < MAX_LED_CHANNELS; ++i)
    {
        /* prepare new queue message */
        txMessage.target_duty[i] = target_duty[i];
    }

    /* transition duration LEDC_FADE_TIME in ms. */
    txMessage.target_brightness = target_brightness;

    if (queue_can != 0) {
        xQueueSendToBack(queue_can, &txMessage, 100/portTICK_PERIOD_MS);
    }
}

static void can_received_data(app_twai_message_id_t id, uint32_t value)
{
    ESP_LOGW(TAG, "new data received: %u %lu", id, value);
    schedule_config_t * schedule_config = get_schedule_config();

    /* Slave device */
    if (schedule_config->use_sync && !schedule_config->sync_master)
    {
        uint8_t val = (uint8_t)(value / 1000);

        switch (id)
        {
            case APP_TWAI_LED_BRIGHTNESS:
            {
                set_brightness(val,1);
                break;
            }
            case APP_TWAI_LED_DUTY_0:
                set_channel_duty(0, val,1);
                break;
            case APP_TWAI_LED_DUTY_1:
                set_channel_duty(1, val,1);
                break;
            case APP_TWAI_LED_DUTY_2:
                set_channel_duty(2, val,1);
                break;
            case APP_TWAI_LED_DUTY_3:
                set_channel_duty(3, val,1);
                break;
            case APP_TWAI_LED_DUTY_4:
                set_channel_duty(4, val,1);
                break;
            case APP_TWAI_LED_DUTY_5:
                set_channel_duty(5, val,1);
                break;
            case APP_TWAI_LED_DUTY_6:
                set_channel_duty(6, val,1);
                break;
            case APP_TWAI_LED_DUTY_7:
                set_channel_duty(7, val,1);
                break;
            default:
                break;
        }
    }
}

_Noreturn void can_sync_task()
{
    x_light_message_t rxMessage;
    queue_can = xQueueCreate( 2, sizeof( x_light_message_t ) );

    schedule_config_t * schedule_config = get_schedule_config();

    uint8_t is_master = schedule_config->use_sync && schedule_config->sync_master;
    app_twai_init(is_master);
    app_twai_register_callback(can_received_data);

    for(;;)
    {
        if (xQueueReceive(queue_can, &rxMessage, 100 / portTICK_PERIOD_MS))
        {
            ESP_LOGD(TAG, "new can sync queue --------- ");
            ESP_LOGD(TAG, "brightness: %f", rxMessage.target_brightness);
            ESP_LOGD(TAG, "duty:");

//            for (uint8_t i = 0; i < MAX_LED_CHANNELS; ++i)
//            {
//                printf("%f ", rxMessage.target_duty[i]);
//            }
//            printf("\n");

            /* Only master send status */
            if (schedule_config->use_sync && schedule_config->sync_master)
            {
                /* Brightness */
                uint32_t brightness = (uint32_t)(rxMessage.target_brightness * 1000.0);

                app_twai_message_t tx_msg;
                tx_msg.id = APP_TWAI_LED_BRIGHTNESS;
                tx_msg.length = 4;
                tx_msg.data[0] = (uint8_t)(brightness >> 24);
                tx_msg.data[1] = (uint8_t)(brightness >> 16);
                tx_msg.data[2] = (uint8_t)(brightness >> 8);
                tx_msg.data[3] = (uint8_t) brightness;

                app_twai_send(&tx_msg);

                /* Channel DUTY */
                for (uint8_t i = 0; i < MAX_LED_CHANNELS; ++i)
                {
                    uint32_t duty = (uint32_t)(rxMessage.target_duty[i] * 1000.0);

                    tx_msg.id = APP_TWAI_LED_DUTY_0 + i;
                    tx_msg.length = 4;
                    tx_msg.data[0] = (uint8_t)(duty >> 24);
                    tx_msg.data[1] = (uint8_t)(duty >> 16);
                    tx_msg.data[2] = (uint8_t)(duty >> 8);
                    tx_msg.data[3] = (uint8_t) duty;

                    app_twai_send(&tx_msg);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}