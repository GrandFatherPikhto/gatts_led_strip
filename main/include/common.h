#pragma once

/* include guard
#ifndef __COMMON_H__
#define __COMMON_H__

#endif 
*/

#include <stdio.h>
#include <stddef.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "led_strip.h"
#include "led_strip_rmt_ws2812.h"
#include "gatts.h"
#include "timer_group.h"

#include "hsvrgb.h"

#define DEVICE_NAME "LED_STRIP"

#define RMT_TX_GPIO        18
#define STRIP_LED_NUMBER   16
#define TAIL_LED_NUMBER     5
#define RMT_TX_CHANNEL    RMT_CHANNEL_3
#define CHASE_SPEED_MS     10
