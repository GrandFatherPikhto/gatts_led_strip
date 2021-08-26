#pragma once

#include "led_strip_rmt_ws2812.h"
#include "esp_gatt_common_api.h"
#include "driver/rmt.h"

#ifndef __LED_STRIP_H__
#define __LED_STRIP_H__

enum {
    STRIP_REGIME_OFF,
    STRIP_REGIME_ALL,
    STRIP_REGIME_RUN
};


/** Указатель на объект управления светодиодной ленты */
extern led_strip_t *strip;
/** Режим работы светодиодов */
extern uint8_t regime;

bool init_strip();
bool set_strip_color(const uint8_t color[4]);
bool set_regime(uint8_t regime);
void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);
void led_strip_next();

#endif  // __LED_STRIP_H__