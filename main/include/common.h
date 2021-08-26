#pragma once

/* include guard
#ifndef __COMMON_H__
#define __COMMON_H__

#endif 
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "led_strip.h"
#include "led_strip_rmt_ws2812.h"
#include "gatt.h"
#include "timer_group.h"

#define GATTS_TABLE_TAG "BLINKER"
#define RMT_TAG "LedStrip"

#define RMT_TX_GPIO       18
#define STRIP_LED_NUMBER  16
#define STRIP_LED_TAIL     5
#define RMT_TX_CHANNEL    RMT_CHANNEL_3
