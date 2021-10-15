#include "common.h"

/** Объект управления лентой */
led_strip_t *strip        = NULL;

/** Параметры управления режимами работы адресной ленты */
LS_PARAMS params = {
    .regime = LED_STRIP_REGIME_OFF,
    .color   = { 
        .red   = 0x00,
        .green = 0x00,
        .blue  = 0x00
     },
    .f_brightness     = 100.0,
    .f_chase_speed    = 100.0,
    .f_tail_length    = 100.0,
    .brightness       = 0xFF,
    .chase_speed_ms   = pdMS_TO_TICKS(MIN_DELAY_MS),
    .tail_length      = TAIL_LED_NUMBER,
    .led_strip_length = STRIP_LED_NUMBER,
    .blink_period     = { 25, 25 }
};

/**
 * 
 **/
bool init_strip() {
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(RMT_TX_GPIO, RMT_TX_CHANNEL);
    // set counter clock to 40MHz
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    // install ws2812 driver
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(params.led_strip_length, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);

    if (!strip) {
        ESP_LOGE(LED_STRIP_TAG, "install WS2812 driver failed");
        return false;
    }
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, pdMS_TO_TICKS(MIN_DELAY_MS)));
    // Show simple rainbow chasing pattern
    ESP_LOGI(LED_STRIP_TAG, "LED Rainbow Chase Start");

    led_strip_set_regime(LED_STRIP_REGIME_OFF, 1);

    return true;
}

/**
 * Для Little Endian
 */
float unpack_float(uint8_t *data) {
    uint32_t tmp = (
        (data[0] << 24) |
        (data[1] << 16) |
        (data[2] <<  8) |
         data[3]
    );

    return *(float *) &tmp;
}

/**
 * Для Little Endian
 */
uint32_t unpack_uint32_t(uint8_t *data) {
    uint32_t tmp = (
        (data[0] << 24) |
        (data[1] << 16) |
        (data[2] <<  8) |
         data[3]
    );
    return tmp;
}

/**
 * 
 */
bool led_strip_set_off() {
    ESP_ERROR_CHECK(strip->clear(strip, pdMS_TO_TICKS(MIN_DELAY_MS)));
    ESP_ERROR_CHECK(strip->refresh(strip, MIN_DELAY_MS));
    printf("Выключили\n");

    vTaskDelay(pdMS_TO_TICKS(MIN_DELAY_MS));

    return true;
}

/**
 * 
 */
bool led_strip_refresh_color() {
    // esp_log_buffer_hex("Новый цвет", led_strip_color, COLOR_LEN);
    for(int i = 0; i < STRIP_LED_NUMBER; i++) {
        ESP_ERROR_CHECK(strip->set_pixel(strip, i, 
            params.color.red, params.color.green, params.color.blue));
    }

    vTaskDelay(pdMS_TO_TICKS(MIN_DELAY_MS));
    // Flush RGB values to LEDs
    ESP_ERROR_CHECK(strip->refresh(strip, pdMS_TO_TICKS(MIN_DELAY_MS)));

    return true;
}

/**
 * 
 */
bool led_strip_set_on() {
    strip->clear(strip, 50);
    vTaskDelay(MIN_DELAY_MS);
    if(params.regime != LED_STRIP_REGIME_OFF) {
        led_strip_refresh_color();
    }

    return true;
}

/**
 * 
 **/
void led_strip_set_regime(uint8_t *data, uint8_t len) {
    if(data) {
        params.regime = data[0];
        printf("Режим: %d\n", params.regime);
        switch(params.regime) {
            case LED_STRIP_REGIME_ALL:
                led_strip_set_on();
                break;
            
            case LED_STRIP_REGIME_OFF:
                led_strip_set_off();
                break;

            default:
                
            break;
        }
    }
}

/**
 * Устанавливает цвет свечения ленты светодиодов
 */
void led_strip_set_color(uint8_t *data, uint8_t len) {
    params.color.red   = data[1];
    params.color.green = data[2];
    params.color.blue  = data[3];
    // printf("color: %02x%02x%02x\n", data[2], data[1], data[0]);
    if(params.regime != LED_STRIP_REGIME_OFF) {
        led_strip_refresh_color();
    }
}

void led_strip_set_brightness(uint8_t *data, uint8_t len) {
    // memcpy(&params.f_brightness, data, 4);
    params.f_brightness = unpack_float(data);
    params.brightness = round(2.55 * params.f_brightness);
    // printf("Яркость: %f %d\n", params.f_brightness, params.brightness);
}

void led_strip_set_chase_speed(uint8_t *data, uint8_t len) {
    params.f_chase_speed = unpack_float(data);
    params.chase_speed_ms = (100.0 - params.f_chase_speed) * SPEED_COEFF;
    if (params.chase_speed_ms < 10) params.chase_speed_ms = 10;
    // printf("Скорость: %02x%02x%02x%02x %f %d мс\n", data[0], data[1], data[2], data[3], params.f_chase_speed, params.chase_speed_ms);
}

void led_strip_set_tail_length(uint8_t *data, uint8_t len) {
    params.f_tail_length = unpack_float(data);
    params.tail_length = round(params.led_strip_length * params.f_tail_length / 100.0);
    // printf("Хвост: %d\n", params.tail_length);
}

void led_strip_set_length(uint8_t *data, uint8_t len) {
    params.led_strip_length = unpack_uint32_t(data);
    // printf("Лента: %d\n", params.led_strip_length);
}

void led_strip_set_frequency(uint8_t *data, uint8_t len) {
    params.f_frequency = unpack_float(data);
    params.blink_period[0] = round(500.0/params.f_frequency);
    params.blink_period[1] = round(500.0/params.f_frequency);
    // printf("Частота: %d/%d\n", params.blink_period[0], params.blink_period[1]);
}


/**
 * 
 */
void led_strip_next_tag() {
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    uint32_t hue;
    static uint32_t start_rgb = 0;

    for (int i = 0; i < 3; i++) {
        for (int j = i; j < STRIP_LED_NUMBER; j += 3) {
            // Build RGB values
            hue = j * 360 / STRIP_LED_NUMBER + start_rgb;
            led_strip_hsv2rgb(hue, 100, params.brightness, &red, &green, &blue);
            // Write RGB values to strip driver
            ESP_ERROR_CHECK(strip->set_pixel(strip, j, red, green, blue));
        }
        // Flush RGB values to LEDs
        ESP_ERROR_CHECK(strip->refresh(strip, MIN_DELAY_MS));
        vTaskDelay(pdMS_TO_TICKS(params.chase_speed_ms));
        strip->clear(strip, pdMS_TO_TICKS(MIN_DELAY_MS));
        vTaskDelay(pdMS_TO_TICKS(MIN_DELAY_MS));
    }
    start_rgb += 60;
}



/**
 * 
 */
void led_strip_next_water() {
    static uint8_t value = 0;
    bool direct = true;
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    uint32_t hue;
    static uint32_t start_rgb = 0;
    for(int i = 0; i < STRIP_LED_NUMBER; i++) {
        hue = 360 / STRIP_LED_NUMBER + start_rgb;
        led_strip_hsv2rgb(hue + i, 100, value, &red, &green, &blue);
        ESP_ERROR_CHECK(strip->set_pixel(strip, i, red, green, blue));
        if(direct) {
            value +=2;
            if(value >= params.brightness)
                direct = false;
        } else {
            value -= 2;
            if(value <= 10) {
                direct = true;
            }
        }
        
    }

    ESP_ERROR_CHECK(strip->refresh(strip, pdMS_TO_TICKS(MIN_DELAY_MS)));
    vTaskDelay(pdMS_TO_TICKS(params.chase_speed_ms));
    start_rgb ++;
}

/**
 * 
 */
void led_strip_next_tail() {
    static uint16_t num = 0;
    uint16_t pos = 0;

    for(int i = num; i < num + params.led_strip_length; i++) {
        pos = i % params.led_strip_length;
        if(i >= num && i <= num + params.tail_length) {
            ESP_ERROR_CHECK(strip->set_pixel(strip, pos, params.color.red, params.color.green, params.color.blue));
        } else {
            ESP_ERROR_CHECK(strip->set_pixel(strip, pos, 0, 0, 0));
        }
    }

    num ++;
    num %= params.led_strip_length;

    ESP_ERROR_CHECK(strip->refresh(strip, pdMS_TO_TICKS(MIN_DELAY_MS)));
    vTaskDelay(pdMS_TO_TICKS(params.chase_speed_ms));
}

/**
 * 
 */
void led_strip_next_blink() {
    static bool state = false;
    ESP_ERROR_CHECK(strip->refresh(strip, pdMS_TO_TICKS(MIN_DELAY_MS)));
    if(state) {
        for(int i = 0; i < params.led_strip_length; i++ ) {
            ESP_ERROR_CHECK(strip->set_pixel(strip, i, params.color.red, params.color.green, params.color.blue));
        }
        ESP_ERROR_CHECK(strip->refresh(strip, pdMS_TO_TICKS(MIN_DELAY_MS)));
        vTaskDelay(pdMS_TO_TICKS(params.blink_period[0]));
    } else {
        strip->clear(strip, MIN_DELAY_MS);
        ESP_ERROR_CHECK(strip->refresh(strip, pdMS_TO_TICKS(MIN_DELAY_MS)));
        vTaskDelay(pdMS_TO_TICKS(params.blink_period[1]));
    }
    state = !state;
}

/**
 * 
 **/
void led_strip_next() {
    switch (params.regime) {
        case LED_STRIP_REGIME_TAG:
            led_strip_next_tag ();
            break;
        
        case LED_STRIP_REGIME_WHATER:
            led_strip_next_water();
            break;

        case LED_STRIP_REGIME_TAIL:
            led_strip_next_tail();
            break;

        case LED_STRIP_REGIME_BLINK:
            led_strip_next_blink();
            break;

        default:
            vTaskDelay(pdMS_TO_TICKS(MIN_DELAY_MS));
            break;
    }
}
