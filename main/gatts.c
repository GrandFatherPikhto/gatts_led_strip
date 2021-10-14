#include "common.h"

 uint8_t adv_config_done = 0;

 uint16_t led_strip_handle_table[LED_STRIP_IDX_NB];

 uint8_t test_manufacturer[3]={'R', 'Q', 'D'};

 uint8_t sec_service_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x18, 0x0D, 0x00, 0x00,
};

// Конфигурация рекламмных данных
 esp_ble_adv_data_t led_strip_adv_config = {
    .set_scan_rsp = false,
    .include_txpower = true,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(sec_service_uuid),
    .p_service_uuid = sec_service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
// Конфигурация данных ответа на сканирование
 esp_ble_adv_data_t led_strip_scan_rsp_config = {
    .set_scan_rsp = true,
    .include_name = true,
    .manufacturer_len = sizeof(test_manufacturer),
    .p_manufacturer_data = test_manufacturer,
};

 esp_ble_adv_params_t led_strip_adv_params = {
    .adv_int_min        = 0x100,
    .adv_int_max        = 0x100,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_RANDOM,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

 void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/** Один профиль на основе GATT, один app_id и один gatts_if. 
 *  Этот массив содержит gatts_if, и возвращается по событию ESP_GATTS_REG_EVT
 */
 struct gatts_profile_inst ledstrip_profile_tab[LED_STRIP_PROFILE_NUM] = {
    [LED_STRIP_APP_IDX] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

/*
 *  Аттрибуты профиля Led Strip
 ****************************************************************************************
 */
#define CHAR_DECLARATION_SIZE   (sizeof(uint8_t))
/** UIID сервиса светодиодной ленты */
 const uint8_t GATTS_SERVICE_UUID_LED_STRIP [16] = { 0xd4, 0x5a, 0x0b, 0x91, 0x01, 0x01, 0x46, 0xa0, 0x4b, 0x5c, 0x18, 0x64, 0xFF, 0x00, 0x00, 0x00 };
 /** UUID характеристики режима работы свечения светодиодной ленты */
 const uint8_t GATTS_CHAR_UUID_REGIME       [16] = { 0xd4, 0x5a, 0x0b, 0x91, 0x01, 0x01, 0x46, 0xa0, 0x4b, 0x5c, 0x18, 0x64, 0x01, 0xFF, 0x00, 0x00 };
 /** UUID характеристики цвета светодиодной ленты */
 const uint8_t GATTS_CHAR_UUID_COLOR        [16] = { 0xd4, 0x5a, 0x0b, 0x91, 0x01, 0x01, 0x46, 0xa0, 0x4b, 0x5c, 0x18, 0x64, 0x02, 0xFF, 0x00, 0x00 };
 /** UUID характеристики цвета светодиодной ленты */  
 const uint8_t GATTS_CHAR_UUID_BRIGHTNESS   [16] = { 0xd4, 0x5a, 0x0b, 0x91, 0x01, 0x01, 0x46, 0xa0, 0x4b, 0x5c, 0x18, 0x64, 0x03, 0xFF, 0x00, 0x00 };
 /** UUID характеристики цвета светодиодной ленты */  
 const uint8_t GATTS_CHAR_UUID_SPEED        [16] = { 0xd4, 0x5a, 0x0b, 0x91, 0x01, 0x01, 0x46, 0xa0, 0x4b, 0x5c, 0x18, 0x64, 0x04, 0xFF, 0x00, 0x00 };
 /** UUID характеристики цвета светодиодной ленты */  
 const uint8_t GATTS_CHAR_UUID_LENGTH       [16] = { 0xd4, 0x5a, 0x0b, 0x91, 0x01, 0x01, 0x46, 0xa0, 0x4b, 0x5c, 0x18, 0x64, 0x05, 0xFF, 0x00, 0x00 };
 /** UUID характеристики цвета светодиодной ленты */
 const uint8_t GATTS_CHAR_UUID_FREQUENCY    [16] = { 0xd4, 0x5a, 0x0b, 0x91, 0x01, 0x01, 0x46, 0xa0, 0x4b, 0x5c, 0x18, 0x64, 0x06, 0xFF, 0x00, 0x00 };


 const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
 const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
 const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
 const uint8_t  char_prop_read_write_notify  = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
 
 const uint8_t  regime_ccc     [2] = {0x01, 0x01};
 const uint8_t  color_ccc      [2] = {0x01, 0x01};
 const uint8_t  brightness_ccc [2] = {0x01, 0x01};
 const uint8_t  length_ccc     [2] = {0x01, 0x01};
 const uint8_t  speed_ccc      [2] = {0x01, 0x01};
 const uint8_t  frequency_ccc  [2] = {0x01, 0x01};

 const uint8_t  regime_value     [1] = { 0x00 };
 const uint8_t  color_value      [4] = { 0x00, 0x00, 0x00, 0x00 };
 const uint8_t  brightness_value [4] = { 0x00, 0x00, 0x00, 0x00 };
 const uint8_t  length_value     [4] = { 0x00, 0x00, 0x00, 0x00 };
 const uint8_t  speed_value      [4] = { 0x00, 0x00, 0x00, 0x00 };
 const uint8_t  blink_value      [4] = { 0x00, 0x00, 0x00, 0x00 };
 const uint8_t  frequency_value  [4] = { 0x00, 0x00, 0x00, 0x00 };
 
 /** Режим работы светодиодной ленты */
uint8_t led_strip_regime = LED_STRIP_REGIME_OFF;
/** Период мерцания ленты */
uint16_t blink_period [2] = { 40, 10 };

uint8_t ls_regime     [1] = { 0x00 };
uint8_t ls_color      [4] = { 0x23, 0x00, 0x00, 0x00 };
uint8_t ls_speed      [4] = { 0x00, 0x00, 0x00, 0x00 };
uint8_t ls_length     [4] = { 0x00, 0x00, 0x00, 0x00 };
uint8_t ls_brightness [1] = { 0x00 };
uint8_t ls_frequency  [4] = { 0x00, 0x00, 0x00, 0x00 };


/**
 * Полное описание Базы Данных аттрибутов LedStrip
 */
const esp_gatts_attr_db_t led_strip_gatt_db[LED_STRIP_IDX_NB] =
{
    /* Объявление сервиса Мигалки */
    [IDX_LED_STRIP_SVC]    =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_LED_STRIP), (uint8_t *)&GATTS_SERVICE_UUID_LED_STRIP}},
    // ================================ Режим Работы Ленты
    /* Характеристика Режима */
    [IDX_CHAR_REGIME]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Значение характеристики Режима Мигания */
    [IDX_CHAR_VAL_REGIME]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)&GATTS_CHAR_UUID_REGIME, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(regime_value), (uint8_t *)regime_value}},

    /* Конфигурационный Дескриптор Клиента */
    [IDX_CHAR_CFG_REGIME]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(regime_ccc), (uint8_t *)regime_ccc}},

    // ================================ Цвет Ленты
    /* Характеристика Цвета Мигания */
    [IDX_CHAR_COLOR]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Значение Характеристики Цвета Мигания */
    [IDX_CHAR_VAL_COLOR] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)&GATTS_CHAR_UUID_COLOR, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(color_value), (uint8_t *)color_value}},

    /* Конфигурационный Дескриптор Клиента */
    [IDX_CHAR_CFG_COLOR]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(color_ccc), (uint8_t *)color_ccc}},

    // ================================ Яркость Светимости Ленты
    /* Характеристика Режима Мигания */
    [IDX_CHAR_BRIGHTNESS]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Значение характеристики Режима Мигания */
    [IDX_CHAR_VAL_BRIGHTNESS]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)&GATTS_CHAR_UUID_BRIGHTNESS, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(brightness_value), (uint8_t *)brightness_value}},

    /* Конфигурационный Дескриптор Клиента */
    [IDX_CHAR_CFG_BRIGHTNESS]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(brightness_ccc), (uint8_t *)brightness_ccc}},

    // ================================ Скорость перемещения эффектов
    /* Характеристика Режима Мигания */
    [IDX_CHAR_SPEED]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Значение характеристики Режима Мигания */
    [IDX_CHAR_VAL_SPEED]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)&GATTS_CHAR_UUID_SPEED, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(speed_value), (uint8_t *)speed_value}},

    /* Конфигурационный Дескриптор Клиента */
    [IDX_CHAR_CFG_SPEED]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(speed_ccc), (uint8_t *)speed_ccc}},

    // ================================ Длина (в %) светящегося сегмента
    /* Характеристика Режима Мигания */
    [IDX_CHAR_LENGTH]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Значение характеристики Режима Мигания */
    [IDX_CHAR_VAL_LENGTH]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)&GATTS_CHAR_UUID_LENGTH, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(length_value), (uint8_t *)length_value}},

    /* Конфигурационный Дескриптор Клиента */
    [IDX_CHAR_CFG_LENGTH]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(length_ccc), (uint8_t *)length_ccc}},


    // ================================ Частота Мигания Ленты
    /* Характеристика Режима Мигания */
    [IDX_CHAR_FREQUENCY]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Значение характеристики Режима Мигания */
    [IDX_CHAR_VAL_FREQUENCY]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)&GATTS_CHAR_UUID_FREQUENCY, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(frequency_value), (uint8_t *)frequency_value}},

    /* Конфигурационный Дескриптор Клиента */
    [IDX_CHAR_CFG_FREQUENCY]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(frequency_ccc), (uint8_t *)frequency_ccc}}

};

/**
 * Обработка ключей для преобразования их в строку
 */
char *esp_key_type_to_str(esp_ble_key_type_t key_type)
{
   char *key_str = NULL;
   switch(key_type) {
    case ESP_LE_KEY_NONE:
        key_str = "ESP_LE_KEY_NONE";
        break;
    case ESP_LE_KEY_PENC:
        key_str = "ESP_LE_KEY_PENC";
        break;
    case ESP_LE_KEY_PID:
        key_str = "ESP_LE_KEY_PID";
        break;
    case ESP_LE_KEY_PCSRK:
        key_str = "ESP_LE_KEY_PCSRK";
        break;
    case ESP_LE_KEY_PLK:
        key_str = "ESP_LE_KEY_PLK";
        break;
    case ESP_LE_KEY_LLK:
        key_str = "ESP_LE_KEY_LLK";
        break;
    case ESP_LE_KEY_LENC:
        key_str = "ESP_LE_KEY_LENC";
        break;
    case ESP_LE_KEY_LID:
        key_str = "ESP_LE_KEY_LID";
        break;
    case ESP_LE_KEY_LCSRK:
        key_str = "ESP_LE_KEY_LCSRK";
        break;
    default:
        key_str = "INVALID BLE KEY TYPE";
        break;

   }

   return key_str;
}

/**
 * Преобразование запроса на авторизацию в строковое представление.
 */
char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req)
{
   char *auth_str = NULL;
   switch(auth_req) {
    case ESP_LE_AUTH_NO_BOND:
        auth_str = "ESP_LE_AUTH_NO_BOND";
        break;
    case ESP_LE_AUTH_BOND:
        auth_str = "ESP_LE_AUTH_BOND";
        break;
    case ESP_LE_AUTH_REQ_MITM:
        auth_str = "ESP_LE_AUTH_REQ_MITM";
        break;
    case ESP_LE_AUTH_REQ_BOND_MITM:
        auth_str = "ESP_LE_AUTH_REQ_BOND_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_ONLY:
        auth_str = "ESP_LE_AUTH_REQ_SC_ONLY";
        break;
    case ESP_LE_AUTH_REQ_SC_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_BOND";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM_BOND";
        break;
    default:
        auth_str = "INVALID BLE AUTH REQ";
        break;
   }

   return auth_str;
}

/**
 * Показать связанные устройства
 */
void show_bonded_devices(void)
{
    int dev_num = esp_ble_get_bond_device_num();

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    ESP_LOGI(GATTS_TABLE_TAG, "Число связанных устройств : %d\n", dev_num);

    ESP_LOGI(GATTS_TABLE_TAG, "Список связанных устройств : %d\n", dev_num);
    for (int i = 0; i < dev_num; i++) {
        esp_log_buffer_hex(GATTS_TABLE_TAG, (void *)dev_list[i].bd_addr, sizeof(esp_bd_addr_t));
    }

    free(dev_list);
}

/**
 * Удалить все связанные устройства
 */
void __attribute__((unused)) remove_all_bonded_devices(void)
{
    int dev_num = esp_ble_get_bond_device_num();

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    for (int i = 0; i < dev_num; i++) {
        esp_ble_remove_bond_device(dev_list[i].bd_addr);
    }

    free(dev_list);
}

/**
 * Обработчик GAP-событий.
 */
void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    ESP_LOGV(GATTS_TABLE_TAG, "GAP_EVT, event %d\n", event);

    switch (event) {
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
        if (adv_config_done == 0){
            esp_ble_gap_start_advertising(&led_strip_adv_params);
        }
        break;
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~ADV_CONFIG_FLAG);
        if (adv_config_done == 0){
            esp_ble_gap_start_advertising(&led_strip_adv_params);
        }
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        /* Cобытие завершения запуска рекламы, указывающее на успешный или неудачный запуск рекламы */
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTS_TABLE_TAG, "advertising start failed, error status = %x", param->adv_start_cmpl.status);
            break;
        }
        ESP_LOGI(GATTS_TABLE_TAG, "advertising start success");
        break;
    case ESP_GAP_BLE_PASSKEY_REQ_EVT:                           /* Событие запроса ключа доступа (passkey) */
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_PASSKEY_REQ_EVT");
        /* Вызовите следующую функцию, чтобы ввести ключ доступа, который отображается на удаленном устройстве. */
        //esp_ble_passkey_reply(ledstrip_profile_tab[LED_STRIP_APP_IDX].remote_bda, true, 0x00);
        break;
    case ESP_GAP_BLE_OOB_REQ_EVT: {
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_OOB_REQ_EVT");
        uint8_t tk[16] = {1}; //If you paired with OOB, both devices need to use the same tk
        esp_ble_oob_req_reply(param->ble_security.ble_req.bd_addr, tk, sizeof(tk));
        break;
    }
    case ESP_GAP_BLE_LOCAL_IR_EVT:                               /* BLE local IR event */
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_LOCAL_IR_EVT");
        break;
    case ESP_GAP_BLE_LOCAL_ER_EVT:                               /* BLE local ER event */
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_LOCAL_ER_EVT");
        break;
    case ESP_GAP_BLE_NC_REQ_EVT:
        /* Приложение получит это событие, если IO имеет возможность DisplayYesNO, 
        а IO однорангового устройства также имеет возможность DisplayYesNo. 
        Покажите номер ключа доступа пользователю, чтобы подтвердить его номером, 
        отображаемым одноранговым устройством. */
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_NC_REQ_EVT, the passkey Notify number:%d", param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_SEC_REQ_EVT:
        /* Отправить положительный (истинный) ответ безопасности на одноранговое устройство, чтобы принять запрос безопасности.
           Если не принять запрос безопасности, следует отправить ответ безопасности с отрицательным (ложным) значением принятия. */
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;
    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:  ///приложение получит это событие, когда IO имеет возможность вывода, а IO однорангового устройства имеет возможность ввода.
        /* показать номер ключа доступа пользователю, чтобы ввести его в одноранговое устройство. */
        ESP_LOGI(GATTS_TABLE_TAG, "The passkey Notify number:%06d", param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_KEY_EVT:
        //показывает пользователю информацию о ключе BLE с одноранговым устройством.
        ESP_LOGI(GATTS_TABLE_TAG, "key type = %s", esp_key_type_to_str(param->ble_security.ble_key.key_type));
        break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT: {
        esp_bd_addr_t bd_addr;
        memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTS_TABLE_TAG, "remote BD_ADDR: %08x%04x",\
                (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                (bd_addr[4] << 8) + bd_addr[5]);
        ESP_LOGI(GATTS_TABLE_TAG, "address type = %d", param->ble_security.auth_cmpl.addr_type);
        ESP_LOGI(GATTS_TABLE_TAG, "pair status = %s",param->ble_security.auth_cmpl.success ? "success" : "fail");
        if(!param->ble_security.auth_cmpl.success) {
            ESP_LOGI(GATTS_TABLE_TAG, "fail reason = 0x%x",param->ble_security.auth_cmpl.fail_reason);
        } else {
            ESP_LOGI(GATTS_TABLE_TAG, "auth mode = %s",esp_auth_req_to_str(param->ble_security.auth_cmpl.auth_mode));
        }
        show_bonded_devices();
        break;
    }
    case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT: {
        ESP_LOGD(GATTS_TABLE_TAG, "ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT status = %d", param->remove_bond_dev_cmpl.status);
        ESP_LOGI(GATTS_TABLE_TAG, "ESP_GAP_BLE_REMOVE_BOND_DEV");
        ESP_LOGI(GATTS_TABLE_TAG, "-----ESP_GAP_BLE_REMOVE_BOND_DEV----");
        esp_log_buffer_hex(GATTS_TABLE_TAG, (void *)param->remove_bond_dev_cmpl.bd_addr, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTS_TABLE_TAG, "------------------------------------");
        break;
    }
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
        if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTS_TABLE_TAG, "config local privacy failed, error status = %x", param->local_privacy_cmpl.status);
            break;
        }

        esp_err_t ret = esp_ble_gap_config_adv_data(&led_strip_adv_config);
        if (ret){
            ESP_LOGE(GATTS_TABLE_TAG, "config adv data failed, error code = %x", ret);
        }else{
            adv_config_done |= ADV_CONFIG_FLAG;
        }

        ret = esp_ble_gap_config_adv_data(&led_strip_scan_rsp_config);
        if (ret){
            ESP_LOGE(GATTS_TABLE_TAG, "config adv data failed, error code = %x", ret);
        }else{
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;
        }

        break;
    default:
        break;
    }
}

void print_handle_table() {
    printf("IDX_LED_STRIP_SVC: %d\n", led_strip_handle_table[IDX_LED_STRIP_SVC]);
    printf("IDX_CHAR_REGIME: %d\n", led_strip_handle_table[IDX_CHAR_REGIME]);
    
    printf("IDX_CHAR_VAL_REGIME %d\n", led_strip_handle_table[IDX_CHAR_VAL_REGIME]);
    printf("IDX_CHAR_CFG_REGIME %d\n", led_strip_handle_table[IDX_CHAR_CFG_REGIME]);

    printf("IDX_CHAR_COLOR %d\n", led_strip_handle_table[IDX_CHAR_COLOR]);
    printf("IDX_CHAR_VAL_COLOR %d\n", led_strip_handle_table[IDX_CHAR_VAL_COLOR]);
    printf("IDX_CHAR_CFG_COLOR %d\n", led_strip_handle_table[IDX_CHAR_CFG_COLOR]);
}

/**
 * Обработчик характеристики.
 **/
void handle_characteristic(esp_gatts_cb_event_t event, // Событие
        esp_gatt_if_t gatts_if, // Интерфейс
        esp_ble_gatts_cb_param_t *param, // Параметры
        uint8_t val_idx, // Индекс значения
        uint8_t cfg_idx, // Индекс конфигурации
        uint8_t **value,  // Значение (куда копировать)
        uint8_t len,     // Длина значения в байтах)
        characteristic_handler_function fun // Функция-обработчик пришедшего значения
) {
    if (led_strip_handle_table[val_idx] == param->write.handle && param->write.len == len) {
        // printf("%d %d\n", sizeof(value), len);
        // memcpy(*value, param->write.value, param->write.len);
        fun(param->write.value, param->write.len);
    }
    
    if (led_strip_handle_table[cfg_idx] == param->write.handle && param->write.len == 2){
        /* Indicate требует подтверждения, а Notify - нет. */
        uint16_t descr_value = param->write.value[1]<<8 | param->write.value[0];
        if (descr_value == 0x0001){
            ESP_LOGI(GATTS_TABLE_TAG, "Уведомление включено"); /// Notify
            uint8_t notify_data[15];
            for (int i = 0; i < sizeof(notify_data); ++i) {
                notify_data[i] = i % 0xff;
            }
            //размер notify_data[] должно быть меньше MTU (Maximum Transfer Unit -- Максимальная Единица Передачи)
            esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, led_strip_handle_table[val_idx],
                                    sizeof(notify_data), notify_data, false);
        } else if (descr_value == 0x0002){
            ESP_LOGI(GATTS_TABLE_TAG, "Подтверждение включено"); /// Indicate
            uint8_t indicate_data[15];
            for (int i = 0; i < sizeof(indicate_data); ++i)
            {
                indicate_data[i] = i % 0xff;
            }
            // Размер indicate_data должен быть меньше, чем размер MTU
            esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, led_strip_handle_table[cfg_idx],
                                sizeof(indicate_data), indicate_data, true);
        } else if (descr_value == 0x0000){
            ESP_LOGI(GATTS_TABLE_TAG, "notify/indicate выключено ");
        } else{
            ESP_LOGE(GATTS_TABLE_TAG, "Неизвестное значение описателя");
            esp_log_buffer_hex(GATTS_TABLE_TAG, param->write.value, param->write.len);
        }
    }
}

/**
 * Читать записанный параметр. Вообще, говоря, надо назвать read-param, но, увы,
 * write.
 */
 void write_param(esp_gatts_cb_event_t event,
        esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    if (!param->write.is_prep){
        // the data length of gattc write  must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
        // ESP_LOGI(GATTS_TABLE_TAG, "GATT_WRITE_EVT, handle = %d, value len = %d", param->write.handle, param->write.len);
        // print_handle_table();
        // esp_log_buffer_hex(GATTS_TABLE_TAG, param->write.value, param->write.len);
        // printf("Байт: %d, Привод: %d [%d]\n", param->write.len, param->write.handle, led_strip_handle_table[IDX_CHAR_VAL_REGIME]);

        handle_characteristic(event, gatts_if, param, 
            IDX_CHAR_VAL_REGIME, IDX_CHAR_CFG_REGIME, 
            (uint8_t**)&ls_regime, 1, 
            &led_strip_set_regime);
        
        handle_characteristic(event, gatts_if, param,
            IDX_CHAR_VAL_COLOR, IDX_CHAR_CFG_COLOR,
            (uint8_t**)&ls_color, 4, 
            &led_strip_set_color);
        
        handle_characteristic(event, gatts_if, param, 
            IDX_CHAR_VAL_BRIGHTNESS, IDX_CHAR_CFG_BRIGHTNESS, 
            (uint8_t**)&ls_brightness, 4, 
            &led_strip_set_brightness);
        
        handle_characteristic(event, gatts_if, param, 
            IDX_CHAR_VAL_SPEED, IDX_CHAR_CFG_SPEED, 
            (uint8_t**)&ls_speed, 4, 
            &led_strip_set_speed);
        
        handle_characteristic(event, gatts_if, param, 
            IDX_CHAR_VAL_LENGTH, IDX_CHAR_CFG_LENGTH, 
            (uint8_t**)&ls_length, 4, 
            &led_strip_set_length);
        
        handle_characteristic(event, gatts_if, param,
            IDX_CHAR_VAL_FREQUENCY, IDX_CHAR_CFG_FREQUENCY, 
            (uint8_t**)&ls_frequency, 4, 
            &led_strip_set_frequency);

        /* send response when param->write.need_rsp is true*/
        if (param->write.need_rsp){
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        }
    } else {
        /* Обработчик подготовки к записи */
        // example_prepare_write_event_env(gatts_if, &prepare_write_env, param);
    }
}

/**
 * Обработчик событий безопасного GATT-профиля
 */
void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGV(GATTS_TABLE_TAG, "event = %x\n",event);
    switch (event) {
        case ESP_GATTS_REG_EVT:
            esp_ble_gap_set_device_name(DEVICE_NAME);
            //Создаёт разрешимый случайный адрес устройства
            esp_ble_gap_config_local_privacy(true);
            esp_ble_gatts_create_attr_tab(led_strip_gatt_db, gatts_if,
                                      LED_STRIP_IDX_NB, HEART_RATE_SVC_INST_ID);
            break;
        case ESP_GATTS_READ_EVT:
            break;
        case ESP_GATTS_WRITE_EVT:
            // ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_WRITE_EVT, write value:");
            // esp_log_buffer_hex(GATTS_TABLE_TAG, param->write.value, param->write.len);
            write_param(event, gatts_if, param);
            break;
        case ESP_GATTS_EXEC_WRITE_EVT:
            break;
        case ESP_GATTS_MTU_EVT:
            break;
        case ESP_GATTS_CONF_EVT:
            break;
        case ESP_GATTS_UNREG_EVT:
            break;
        case ESP_GATTS_DELETE_EVT:
            break;
        case ESP_GATTS_START_EVT:
            break;
        case ESP_GATTS_STOP_EVT:
            break;
        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_CONNECT_EVT");
            /* start security connect with peer device when receive the connect event sent by the master */
            esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
            break;
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);
            /* start advertising again when missing the connect */
            esp_ble_gap_start_advertising(&led_strip_adv_params);
            break;
        case ESP_GATTS_OPEN_EVT:
            break;
        case ESP_GATTS_CANCEL_OPEN_EVT:
            break;
        case ESP_GATTS_CLOSE_EVT:
            break;
        case ESP_GATTS_LISTEN_EVT:
            break;
        case ESP_GATTS_CONGEST_EVT:
            break;
        case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
            ESP_LOGI(GATTS_TABLE_TAG, "The number handle = %x",param->add_attr_tab.num_handle);
            if (param->create.status == ESP_GATT_OK){
                if(param->add_attr_tab.num_handle == LED_STRIP_IDX_NB) {
                    memcpy(led_strip_handle_table, param->add_attr_tab.handles,
                    sizeof(led_strip_handle_table));
                    esp_ble_gatts_start_service(led_strip_handle_table[IDX_LED_STRIP_SVC]);
                }else{
                    ESP_LOGE(GATTS_TABLE_TAG, "Create attribute table abnormally, num_handle (%d) doesn't equal to HRS_IDX_NB(%d)",
                         param->add_attr_tab.num_handle, LED_STRIP_IDX_NB);
                }
            }else{
                ESP_LOGE(GATTS_TABLE_TAG, " Create attribute table failed, error code = %x", param->create.status);
            }
        break;
    }

        default:
           break;
    }
}

/**
 * Обработчик событий GATT
 */
void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param)
{
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            ledstrip_profile_tab[LED_STRIP_APP_IDX].gatts_if = gatts_if;
        } else {
            ESP_LOGI(GATTS_TABLE_TAG, "Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    do {
        int idx;
        for (idx = 0; idx < LED_STRIP_PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gatts_if == ledstrip_profile_tab[idx].gatts_if) {
                if (ledstrip_profile_tab[idx].gatts_cb) {
                    ledstrip_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

/**
 * Запуск GATT-сервера
 */
int init_gatt(void) {
    esp_err_t ret;

    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s init controller failed: %s", __func__, esp_err_to_name(ret));
        return -1;
    }
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return -2;
    }

    ESP_LOGI(GATTS_TABLE_TAG, "%s init bluetooth", __func__);
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return -3;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return -4;
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret){
        ESP_LOGE(GATTS_TABLE_TAG, "gatts register error, error code = %x", ret);
        return -5;
    }
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(GATTS_TABLE_TAG, "gap register error, error code = %x", ret);
        return -6;
    }
    ret = esp_ble_gatts_app_register(LED_STRIP_APP_ID);
    if (ret){
        ESP_LOGE(GATTS_TABLE_TAG, "gatts app register error, error code = %x", ret);
        return -7;
    }

    /* Установите параметры безопасности iocap и auth_req, размер ключа и ключевые параметры ответа ключа инициализации в стек */
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;     //соединение с одноранговым устройством после аутентификации
    esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;           //установить для ввода-вывода значения: Нет вывода, Нет ввода
    uint8_t key_size = 16;      //размер ключа должен составлять 7~16 байт
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    //Установить ключ доступа
    uint32_t passkey = 123456;
    uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    uint8_t oob_support = ESP_BLE_OOB_DISABLE;
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t));
    /* Если ваше устройство BLE действует как ведомое устройство, init_key означает, что вы надеетесь, какие типы ключей ведущего устройства должны передавать вам,
     а ключ ответа означает, какой ключ вы можете передать мастеру;
     Если ваше устройство BLE действует как ведущее устройство, ключ ответа означает, 
     что вы надеетесь, какие типы ключей ведомого устройства должны передавать вам, 
     а ключ инициализации означает, какой ключ вы можете передать ведомому устройству. */
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));

    /* Просто демонстрирует, как сбросить все связанные устройства
     * Ожидает 30 сек, и сбрасывает все связанные устройства
     *
     * vTaskDelay(30000 / portTICK_PERIOD_MS);
     * remove_all_bonded_devices();
     */

    return 0;
}
