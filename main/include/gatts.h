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
#define GATTS_TABLE_TAG "LED_STRIP"

#define LED_STRIP_PROFILE_NUM                     1
#define LED_STRIP_APP_IDX                         0
#define LED_STRIP_APP_ID                          0x55
#define HEART_RATE_SVC_INST_ID                    0

#define ADV_CONFIG_FLAG                           (1 << 0)
#define SCAN_RSP_CONFIG_FLAG                      (1 << 1)

#define PROFILE_NUM                 1
#define PROFILE_APP_IDX             0
#define ESP_APP_ID                  0x57
#define SAMPLE_DEVICE_NAME          "LED_STRIP"
#define SVC_INST_ID                 0

/* The max length of characteristic value. When the GATT client performs a write or prepare write operation,
*  the data length must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
*/
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40
#define PREPARE_BUF_MAX_SIZE        1024
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))
#define EXAMPLE_CHASE_SPEED_MS      (10)

#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

typedef void (*characteristic_handler_function) (uint8_t *data, uint8_t length);

/* Attributes State Machine */
enum
{
    IDX_LED_STRIP_SVC,

    IDX_CHAR_REGIME,
    IDX_CHAR_VAL_REGIME,
    IDX_CHAR_CFG_REGIME,

    IDX_CHAR_COLOR,
    IDX_CHAR_VAL_COLOR,
    IDX_CHAR_CFG_COLOR,

    IDX_CHAR_BRIGHTNESS,
    IDX_CHAR_VAL_BRIGHTNESS,
    IDX_CHAR_CFG_BRIGHTNESS,

    IDX_CHAR_CHASE_SPEED,
    IDX_CHAR_VAL_CHASE_SPEED,
    IDX_CHAR_CFG_CHASE_SPEED,

    IDX_CHAR_TAIL_LENGTH,
    IDX_CHAR_VAL_TAIL_LENGTH,
    IDX_CHAR_CFG_TAIL_LENGTH,

    IDX_CHAR_FREQUENCY,
    IDX_CHAR_VAL_FREQUENCY,
    IDX_CHAR_CFG_FREQUENCY,

    IDX_CHAR_LS_LENGTH,
    IDX_CHAR_VAL_LS_LENGTH,
    IDX_CHAR_CFG_LS_LENGTH,

    LED_STRIP_IDX_NB,
};

int init_gatt();

/*
#endif // __BLINK_GATT_H__
*/