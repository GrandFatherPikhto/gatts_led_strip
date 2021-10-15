#pragma once

#include "led_strip_rmt_ws2812.h"
#include "esp_gatt_common_api.h"
#include "driver/rmt.h"

// #ifndef __LED_STRIP_H__
// #define __LED_STRIP_H__
typedef struct __RGB_COLOR__ {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGB_COLOR;


typedef struct __LS_PARAMS__ {
    uint8_t  regime;
    RGB_COLOR color;
    float    f_brightness;
    float    f_chase_speed;
    float    f_tail_length;
    float    f_frequency;
    uint8_t  brightness;
    uint16_t chase_speed_ms;
    uint16_t tail_length;
    uint16_t led_strip_length;
    uint16_t blink_period[2];
} LS_PARAMS;

#define LED_STRIP_TAG "LedStrip"
#define MIN_DELAY_MS  10
#define SPEED_COEFF   5.0

enum {
    LED_STRIP_REGIME_OFF       = 0x00,
    LED_STRIP_REGIME_ALL       = 0x01,
    LED_STRIP_REGIME_TAG       = 0x02,
    LED_STRIP_REGIME_WHATER    = 0x03,
    LED_STRIP_REGIME_TAIL      = 0x04,
    LED_STRIP_REGIME_BLINK     = 0x05
};

/** Указатель на объект управления светодиодной ленты */
extern led_strip_t *strip;

bool init_strip();

/** Функция-сеттер. Установка режима ленты */
void led_strip_set_regime ( uint8_t *data, uint8_t len );
/** Функция-сеттер. Установка цвета ленты */
void led_strip_set_color  ( uint8_t *data, uint8_t len );
/** */
void led_strip_set_brightness ( uint8_t *data, uint8_t len);
/** */
void led_strip_set_chase_speed ( uint8_t *data, uint8_t len);
/** */
void led_strip_set_tail_length ( uint8_t *data, uint8_t len);
/** Функция-сеттер. Установка частоты мерцания ленты */
void led_strip_set_frequency(uint8_t *data, uint8_t len);
/** */
void led_strip_set_length ( uint8_t *data, uint8_t len);
/** */
void led_strip_next();

// #endif  // __LED_STRIP_H__