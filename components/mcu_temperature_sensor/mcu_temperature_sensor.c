//
// Created by Aleksey Volkov on 6/1/24.
//

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/temperature_sensor.h"

#include "mcu_temperature_sensor.h"

static const char *TAG = "MCU TEMPERATURE SENSOR";
static temperature_sensor_handle_t sensor = NULL;
static bool sensor_installed = false;

void mcu_temperature_sensor_init()
{
    temperature_sensor_config_t sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    ESP_ERROR_CHECK(temperature_sensor_install(&sensor_config, &sensor));
    ESP_ERROR_CHECK(temperature_sensor_enable(sensor));
    sensor_installed = true;
}

float read_mcu_temperature()
{
    float value;
    if (!sensor_installed)
    {
        return -255;
    }
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(sensor, &value));
    return value;
}