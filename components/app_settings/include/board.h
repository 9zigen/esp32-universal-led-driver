#pragma once

#define USE_RTC
#define USE_FAN_PWM
#define USE_FAN_ALWAYS
#define C6_5CH_LED_DRIVER_AIO_V3A
#define C6_5CH_LED_DRIVER_AIO_V3
//#define CUSTOM_7CH_LED_DRIVER_AIO
//#define THINGSBOARD


#if defined(C6_5CH_LED_DRIVER_AIO_V3)
/* ADC */
#define ADC_CURRENT         ADC_CHANNEL_0
#define VIN_CURRENT         ADC_CHANNEL_1
#define NTC_CURRENT         ADC_CHANNEL_3

/* I2C */
#define I2C_SDA             GPIO_NUM_7
#define I2C_SCL             GPIO_NUM_6

/* CAN Bus */
#define CAN_TX              GPIO_NUM_8
#define CAN_RX              GPIO_NUM_10

#define POWER_SWITCH        GPIO_NUM_11

/* WS2812 */
#define WS2812_GBR          1
#define WS2812_LED_IO       GPIO_NUM_2
#define WS2812_LED_NUM      1

/* LEDC Hardware */
#define LEDC_LED_CHANNELS   5
#define LEDC_CH1            GPIO_NUM_12 /* LED DRIVER 1 */
#define LEDC_CH2            GPIO_NUM_13 /* LED DRIVER 2 */
#define LEDC_CH3            GPIO_NUM_21 /* LED DRIVER 3 */
#define LEDC_CH4            GPIO_NUM_20 /* LED DRIVER 4 */
#define LEDC_CH5            GPIO_NUM_19 /* LED DRIVER 5 */
#define LEDC_CH6            GPIO_NUM_23 /* FAN DC Made */
#define LEDC_CH6_ALT        GPIO_NUM_18 /* FAN PWM Mode */

/* Pulse Counter */
#define FAN_TACHOMETER      GPIO_NUM_18 /* FAN Speed Sensor */

#else
#define LED_NUM           0
#endif