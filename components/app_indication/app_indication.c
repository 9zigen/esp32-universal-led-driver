//
// Created by Aleksey Volkov on 23/6/24.
//

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "led_strip_encoder.h"

#include "board.h"
#include "app_indication.h"

#define WHITE {255, 255, 255}
#define RED {255, 0, 0}
#define YELLOW {255, 255, 0}
#define GREEN {0, 128, 0}
#define CYAN {0, 255, 255}
#define BLUE {0, 0, 255}
#define MAGENTA {255, 0, 255}

#define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)

static const char *TAG = "APP INDICATION";
static uint8_t led_strip_pixels[WS2812_LED_NUM * 3];
static pixel_t pixels[WS2812_LED_NUM];

static const pixel_color_t white    = WHITE;
static const pixel_color_t red      = RED;
static const pixel_color_t yellow   = YELLOW;
static const pixel_color_t green    = GREEN;
static const pixel_color_t cyan     = CYAN;
static const pixel_color_t blue     = BLUE;
static const pixel_color_t magenta  = MAGENTA;

rmt_channel_handle_t led_chan = NULL;
rmt_encoder_handle_t led_encoder = NULL;

/* TODO: Web UI control */
static uint8_t brightness = 20; /* Range 0 - 255 */

uint8_t blinks_mode_map[] = {
        0B00000000, /* Off */
        0B11111111, /* On */
        0B00001111, /* Half second blinking */
        0B00000001, /* Short flash once per second */
        0B00000101, /* Two short flashes once a second */
        0B00010101, /* Three short flashes once a second */
        0B01010101  /* Frequent short flashes (4 times per second) */
};

/**
 * @brief Simple helper function, converting HSV color space to RGB color space
 *
 * Wiki: https://en.wikipedia.org/wiki/HSL_and_HSV
 *
 */
void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i) {
        case 0:
            *r = rgb_max;
            *g = rgb_min + rgb_adj;
            *b = rgb_min;
            break;
        case 1:
            *r = rgb_max - rgb_adj;
            *g = rgb_max;
            *b = rgb_min;
            break;
        case 2:
            *r = rgb_min;
            *g = rgb_max;
            *b = rgb_min + rgb_adj;
            break;
        case 3:
            *r = rgb_min;
            *g = rgb_max - rgb_adj;
            *b = rgb_max;
            break;
        case 4:
            *r = rgb_min + rgb_adj;
            *g = rgb_min;
            *b = rgb_max;
            break;
        default:
            *r = rgb_max;
            *g = rgb_min;
            *b = rgb_max - rgb_adj;
            break;
    }
}

static void set_pixel_color(uint8_t idx, pixel_color_t color)
{
    if (idx < WS2812_LED_NUM) {
        led_strip_pixels[idx * 3 + 0] = WS2812_GBR? color.green : color.red;
        led_strip_pixels[idx * 3 + 1] = color.blue;
        led_strip_pixels[idx * 3 + 2] = WS2812_GBR? color.red : color.green;
    }

    rmt_transmit_config_t tx_config = {
            .loop_count = 0, // no transfer loop
    };

    // Flush RGB values to LEDs
    ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config));
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(led_chan, portMAX_DELAY));
}

static void off_pixel(uint8_t idx)
{
    if (idx < WS2812_LED_NUM) {
        pixel_color_t color = {0};
        set_pixel_color(idx, color);
    }
}


static void indication_timer_handler( TimerHandle_t pxTimer )
{
    for (int i = 0; i < WS2812_LED_NUM; ++i)
    {
        uint8_t blink_loop = 0;

        if (pixels[i].blinks_left > 0 && pixels[i].is_active)
        {
            for (int j = 0; j < 7; ++j)
            {
                if(pixels[i].blinks_mode & 1 << (blink_loop & 0x07))
                {
                    set_pixel_color(i, pixels[i].color);
                } else {
                    off_pixel(i);
                }

                blink_loop++;
                vTaskDelay(200 / portTICK_PERIOD_MS);
            }

            if (pixels[i].blinks_left != 255) {
                pixels[i].blinks_left--;
            }
        }

        if (pixels[i].blinks_left == 0) {
            pixels[i].is_active = 0;

            off_pixel(i);
        }
    }
}

void app_indication_set(pixel_color_t color, blinks_mode_t mode, uint8_t count)
{
    for (int i = 0; i < WS2812_LED_NUM; i++)
    {
        pixels[i].use_blinks = 1;
        pixels[i].blinks_mode = blinks_mode_map[mode];
        pixels[i].blinks_left = count;
        pixels[i].is_active = 1;
        pixels[i].color = color;

        /* Brightness */
        pixels[i].color.red = (uint8_t) ((float)pixels[i].color.red * (brightness / 255.0));
        pixels[i].color.green = (uint8_t) ((float)pixels[i].color.green * (brightness / 255.0));
        pixels[i].color.blue = (uint8_t) ((float)pixels[i].color.blue * (brightness / 255.0));
    }
}

void app_indication_set_predefined(app_indication_color_t indication_color, blinks_mode_t mode, uint8_t count)
{
    switch (indication_color)
    {
        case INDICATE_WHITE:
            return app_indication_set(white, mode, count);

        case INDICATE_RED:
            return app_indication_set(red, mode, count);

        case INDICATE_YELLOW:
            return app_indication_set(yellow, mode, count);

        case INDICATE_GREEN:
            return app_indication_set(green, mode, count);

        case INDICATE_CYAN:
            return app_indication_set(cyan, mode, count);

        case INDICATE_BLUE:
            return app_indication_set(blue, mode, count);

        case INDICATE_MAGENTA:
            return app_indication_set(magenta, mode, count);
    }
}

void app_indication_init(void)
{
    ESP_LOGI(TAG, "Create RMT TX channel");
    rmt_tx_channel_config_t tx_chan_config = {
            .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
            .gpio_num = WS2812_LED_IO,
            .mem_block_symbols = 64, // increase the block size can make the LED less flickering
            .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
            .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &led_chan));

    ESP_LOGI(TAG, "Install led strip encoder");
    led_strip_encoder_config_t encoder_config = {
            .resolution = RMT_LED_STRIP_RESOLUTION_HZ,
    };
    ESP_ERROR_CHECK(rmt_new_led_strip_encoder(&encoder_config, &led_encoder));

    ESP_LOGI(TAG, "Enable RMT TX channel");
    ESP_ERROR_CHECK(rmt_enable(led_chan));

    /* 2000ms timer */
    TimerHandle_t xLedTimer = xTimerCreate( "ledTimer", ( 2000 / portTICK_PERIOD_MS), pdTRUE, 0, indication_timer_handler);
    xTimerStart(xLedTimer, 100 / portTICK_PERIOD_MS);
}
