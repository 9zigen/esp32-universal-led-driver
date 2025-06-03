#pragma once

#include <hal/twai_types.h>

typedef enum {
    APP_TWAI_LED_BRIGHTNESS = 0x001,
    APP_TWAI_LED_DUTY_0     = 0x002,
    APP_TWAI_LED_DUTY_1     = 0x003,
    APP_TWAI_LED_DUTY_2     = 0x004,
    APP_TWAI_LED_DUTY_3     = 0x005,
    APP_TWAI_LED_DUTY_4     = 0x006,
    APP_TWAI_LED_DUTY_5     = 0x007,
    APP_TWAI_LED_DUTY_6     = 0x008,
    APP_TWAI_LED_DUTY_7     = 0x009,
    APP_TWAI_LED_MAX        = 0x00A,
} app_twai_message_id_t;

typedef struct {
    app_twai_message_id_t id;
    uint8_t length;
    uint8_t data[TWAI_FRAME_MAX_DLC];
} app_twai_message_t;

typedef void twai_callback_t(app_twai_message_id_t id, uint32_t rx_value);

void app_twai_send(app_twai_message_t *message);
void app_twai_register_callback(twai_callback_t * cb);
void app_twai_init(uint8_t is_master);
