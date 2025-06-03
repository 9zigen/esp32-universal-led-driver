#pragma once

typedef enum {
    LED_OFF,
    LED_ON,
    LED_SLOW_BLINK,
    LED_FAST_BLINK,
    LED_TWO_BLINK,
    LED_THREE_BLINK,
    LED_FOUR_BLINK
} blinks_mode_t;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel_color_t;

typedef struct {
    uint8_t use_pattern; /* TODO */
    uint8_t use_blinks;

    /* Pattern settings */
    uint8_t pattern_id; /* TODO */

    /* Blinks settings */
    blinks_mode_t blinks_mode;
    uint8_t blinks_left;

    /* Common settings */
    uint8_t is_active;
    pixel_color_t color;
} pixel_t;

typedef enum {
    INDICATE_WHITE,
    INDICATE_RED,
    INDICATE_YELLOW,
    INDICATE_GREEN,
    INDICATE_CYAN,
    INDICATE_BLUE,
    INDICATE_MAGENTA
} app_indication_color_t;

void app_indication_set(pixel_color_t color, blinks_mode_t mode, uint8_t count);
void app_indication_set_predefined(app_indication_color_t indication_color, blinks_mode_t mode, uint8_t count);
void app_indication_init();