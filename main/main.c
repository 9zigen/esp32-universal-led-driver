#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include "adc.h"
#include <pwm.h>
#include <pidreg.h>
#include <fanspeed.h>
#include <auth.h>

#include "board.h"
#include "mcp7940.h"
#include "server.h"
#include "monitor.h"
#include "app_settings.h"
#include "rtc.h"
#include "light.h"
#include "main.h"
#include "buttons.h"
#include "thingsboard.h"
#include "connect.h"
#include "udp_multicast.h"
#include "pwm.h"
#include "ota.h"
#include "app_events.h"
#include "mcu_temperature_sensor.h"
#include "mqtt.h"

/* Refactoring */
#include "app_indication.h"
#include "app_twai.h"
#include "can.h"


static const char *TAG="APP";
static uint8_t ota_requested = 0;

esp_err_t upgrade_firmware(void)
{
    services_t * services = get_services();
    char * ota_url_ptr = NULL;
    if (strlen(services->ota_url) > 16)
    {
        ota_url_ptr = services->ota_url;
        ESP_LOGI(TAG, "update firmware from %s", ota_url_ptr);
    }

    /* Set OTA flag to prevent starting web server in event loop */
    ota_requested = 1;

    /* start ota task */
    if (xTaskCreate(&ota_task, "ota_task", 8192, ota_url_ptr, 5, NULL) != pdPASS)
    {
        app_indication_set_predefined(INDICATE_MAGENTA, LED_SLOW_BLINK, 255);
        return ESP_OK;
    } else {
        app_indication_set_predefined(INDICATE_RED, LED_THREE_BLINK, 30);
        return ESP_ERR_NO_MEM;
    }
}

void app_main()
{
    /* POWER SWITCH */
    gpio_set_direction(POWER_SWITCH, GPIO_MODE_OUTPUT);
    gpio_set_level(POWER_SWITCH, 0);

    /* LEDC */
    init_ledc();

    /* NVS Init */
    init_settings();

    /* Notification LED */
    app_indication_init();
    app_indication_set_predefined(INDICATE_YELLOW, LED_SLOW_BLINK, 3);

    /* Force Factory reset */
    gpio_set_direction(GPIO_NUM_9, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_9, GPIO_PULLUP_ONLY);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    if (!gpio_get_level(GPIO_NUM_9))
    {
        erase_settings();

        app_indication_set_predefined(INDICATE_CYAN, LED_FAST_BLINK, 255);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        esp_restart();
    }

    /* Wi-Fi + Web server */
    initialise_wifi(NULL);

    /* Buttons */
    init_buttons();

#ifdef USE_RTC
    /* MCP7940 */
    mcp7940_init();
#endif

    /* MCU Temperature sensor */
    mcu_temperature_sensor_init();

    /* RTC Setup */
    init_clock();

    /* ADC1 */
    init_adc();
    adc_calibrate_load_current();

    /* Events */
    init_events();

    /* Monitor */
    init_monitor();

    /* Light Task */
    ESP_LOGI(TAG, "[APP] free memory before start light task %lu bytes", esp_get_free_heap_size());
    xTaskCreate(&task_light, "task_light", 2048, NULL, 5, NULL);

    /* Light Transition Task */
    ESP_LOGI(TAG, "[APP] free memory before start light transition task %lu bytes", esp_get_free_heap_size());
    xTaskCreate(&task_light_transition, "task_light_transition", 2048, NULL, 5, NULL);

#ifdef USE_FAN_PWM
    /* Fan PID Task */
    ESP_LOGI(TAG, "[APP] free memory before fan pid task %lu bytes", esp_get_free_heap_size());
    //fan_speed_init();
    xTaskCreate(&task_pid_calc, "task_pid_calc", 2048, NULL, 3, NULL);
#endif

    /* web server */
    ESP_LOGI(TAG, "[APP] free memory before start web server %lu bytes", esp_get_free_heap_size());
    start_webserver();

    /* Wait WiFi */
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);

    /* UDP Sync Task */
    xTaskCreate(&mcast_udp_task, "mcast_udp_task", 4096, NULL, 5, NULL);

    /* MQTT Task */
    start_mqtt();

    /* TWAI CAN 2.0 Synchronization task */
    xTaskCreate(&can_sync_task, "can_sync", 4096, NULL, 5, NULL);

    /* ThingsBoard Task */
#ifdef THINGSBOARD
    xTaskCreate(&task_thingsboard, "thingsboard_task", 2048, NULL, 5, NULL);
#endif
}
