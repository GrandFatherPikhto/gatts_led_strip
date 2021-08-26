#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Include Guard
#ifndef __BLINK_GATT_H__
#define __BLINK_GATT_H__
*/

#define PROFILE_NUM                 1
#define PROFILE_APP_IDX             0
#define ESP_APP_ID                  0x57
#define SAMPLE_DEVICE_NAME          "BLINKER"
#define SVC_INST_ID                 0

/* The max length of characteristic value. When the GATT client performs a write or prepare write operation,
*  the data length must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
*/
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500
#define PREPARE_BUF_MAX_SIZE        1024
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))
#define EXAMPLE_CHASE_SPEED_MS      (10)

#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)

extern uint8_t  color_value [4];

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

/* Attributes State Machine */
enum
{
    IDX_SVC_BLINKER,

    IDX_CHAR_COLOR,
    IDX_CHAR_VAL_COLOR,
    IDX_CHAR_CFG_COLOR,

    IDX_CHAR_REGIME,
    IDX_CHAR_VAL_REGIME,
    IDX_CHAR_CFG_REGIME,

    BLINKER_IDX_NB,
};

bool init_gatt();

/*
#endif // __BLINK_GATT_H__
*/