#pragma once

#include "led_strip_rmt_ws2812.h"
#include "esp_gatt_common_api.h"
#include "driver/rmt.h"

// #ifndef __LED_STRIP_H__
// #define __LED_STRIP_H__

#define LED_STRIP_TAG "LedStrip"

#define COLOR_LEN 3
#define SPEED_COEFF 5.0

enum {
    LED_STRIP_REGIME_OFF       = 0x00,
    LED_STRIP_REGIME_ALL       = 0x01,
    LED_STRIP_REGIME_TAG       = 0x02,
    LED_STRIP_REGIME_WHATER    = 0x03,
    LED_STRIP_REGIME_TAIL      = 0x04,
    LED_STRIP_REGIME_BLINK     = 0x05
};

typedef union __FLOAT__ {
    float fval;
    uint8_t bytes[4];
} FLOAT;

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
void led_strip_set_speed ( uint8_t *data, uint8_t len);
/** */
void led_strip_set_length ( uint8_t *data, uint8_t len);
/** Функция-сеттер. Установка частоты мерцания ленты */
void led_strip_set_frequency(uint8_t *data, uint8_t len);

void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);
void led_strip_next();

// #endif  // __LED_STRIP_H__