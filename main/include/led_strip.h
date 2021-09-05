#pragma once

#include "led_strip_rmt_ws2812.h"
#include "esp_gatt_common_api.h"
#include "driver/rmt.h"

// #ifndef __LED_STRIP_H__
// #define __LED_STRIP_H__

#define LED_STRIP_TAG "LedStrip"

#define COLOR_LEN 3

enum {
    LED_STRIP_REGIME_OFF       = 0x0,
    LED_STRIP_REGIME_ALL       = 0x1,
    LED_STRIP_REGIME_TAG       = 0x2,
    LED_STRIP_REGIME_WHATER    = 0x3,
    LED_STRIP_REGIME_TAIL      = 0x4,
    LED_STRIP_REGIME_BLINK     = 0x5
};


/** Указатель на объект управления светодиодной ленты */
extern led_strip_t *strip;

bool init_strip();
bool led_strip_set_color(const uint8_t *color);
bool led_strip_set_regime(uint8_t regime);
void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);
void led_strip_next();

// #endif  // __LED_STRIP_H__