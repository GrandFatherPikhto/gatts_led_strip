#include "common.h"

led_strip_t *strip = NULL;

uint8_t color[3] = { 0x00, 0x00, 0x00 };

float brightness = 0.0;
float speed      = 0.0;
float length     = 0.0;
float frequency  = 0.0;

uint16_t speed_ms = 10;

uint16_t tail_len = 25;

/**
 * @brief Simple helper function, converting HSV color space to RGB color space
 *
 * Wiki: https://en.wikipedia.org/wiki/HSL_and_HSV
 *
 */
void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b) {
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i) {
    case 0:
        *r = rgb_max;
        *g = rgb_min + rgb_adj;
        *b = rgb_min;
        break;
    case 1:
        *r = rgb_max - rgb_adj;
        *g = rgb_max;
        *b = rgb_min;
        break;
    case 2:
        *r = rgb_min;
        *g = rgb_max;
        *b = rgb_min + rgb_adj;
        break;
    case 3:
        *r = rgb_min;
        *g = rgb_max - rgb_adj;
        *b = rgb_max;
        break;
    case 4:
        *r = rgb_min + rgb_adj;
        *g = rgb_min;
        *b = rgb_max;
        break;
    default:
        *r = rgb_max;
        *g = rgb_min;
        *b = rgb_max - rgb_adj;
        break;
    }
}

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
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(STRIP_LED_NUMBER, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);

    if (!strip) {
        ESP_LOGE(LED_STRIP_TAG, "install WS2812 driver failed");
        return false;
    }
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 10));
    // Show simple rainbow chasing pattern
    ESP_LOGI(LED_STRIP_TAG, "LED Rainbow Chase Start");

    led_strip_set_regime(LED_STRIP_REGIME_OFF, 1);

    return true;
}

/**
 * 
 */
bool led_strip_set_off() {
    static bool cleared = false;
    if(cleared == false) {
        strip->clear(strip, 100);
        cleared = true;
    }
    vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));

    return true;
}

/**
 * 
 */
bool led_strip_refresh_color() {
    // esp_log_buffer_hex("Новый цвет", led_strip_color, COLOR_LEN);
    for(int i = 0; i < STRIP_LED_NUMBER; i++) {
        ESP_ERROR_CHECK(strip->set_pixel(strip, i, 
            color[0], color[1], color[2]));
    }

    vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
    // Flush RGB values to LEDs
    ESP_ERROR_CHECK(strip->refresh(strip, CHASE_SPEED_MS));

    return true;
}

/**
 * 
 */
bool led_strip_set_on() {
    strip->clear(strip, 50);
    vTaskDelay(CHASE_SPEED_MS);
    if(led_strip_regime != LED_STRIP_REGIME_OFF) {
        led_strip_refresh_color();
    }

    return true;
}

/**
 * 
 **/
void led_strip_set_regime(uint8_t *data, uint8_t len) {
    if(data) {
    uint8_t regime = data[0];
    printf("Режим: %d\n", regime);
    led_strip_regime = regime;
    switch(led_strip_regime) {
        case LED_STRIP_REGIME_OFF:
            ESP_LOGI(LED_STRIP_TAG, "Очистить ленту. Получен режим 0");
            strip->clear(strip, 100);
            // strip->refresh(strip, 100);
        break;

        case LED_STRIP_REGIME_ALL:
            led_strip_set_on();
        break;

        default:
        break;
    }

    if(led_strip_regime == LED_STRIP_REGIME_OFF) {
        led_strip_set_off();
    } 
    }
}

/**
 * Устанавливает цвет свечения ленты светодиодов
 */
void led_strip_set_color(uint8_t *data, uint8_t len) {
    // esp_log_buffer_hex(LED_STRIP_TAG, led_strip_color, COLOR_LEN);
    color[0] = data[2];
    color[1] = data[1];
    color[2] = data[0];
    if(led_strip_regime != LED_STRIP_REGIME_OFF) {
        led_strip_refresh_color();
    }
}

void led_strip_set_brightness(uint8_t *data, uint8_t len) {
    FLOAT value;
    memcpy(value.bytes, data, 4);
    brightness = value.fval;
    printf("Яркость: %f\n", brightness);
}

void led_strip_set_speed(uint8_t *data, uint8_t len) {
    FLOAT value;
    memcpy(value.bytes, data, 4);
    speed = value.fval;
    speed_ms = (100.0 - speed) * SPEED_COEFF;
    if (speed_ms < 10) speed_ms = 10;
    // printf("Скорость: %f, %d\n", speed, speed_ms);
}

void led_strip_set_length(uint8_t *data, uint8_t len) {
    FLOAT value;
    memcpy(value.bytes, data, 4);
    length = value.fval;
    tail_len = round(STRIP_LED_NUMBER * length / 100.0);
    printf("Длина: %f\n", length);
}

void led_strip_set_frequency(uint8_t *data, uint8_t len) {
    FLOAT value;
    memcpy(value.bytes, data, 4);
    blink_period[0] = round(500.0/value.fval);
    blink_period[1] = round(500.0/value.fval);
    frequency = value.fval;
    printf("Частота: %f\n", frequency);
}


/**
 * 
 */
void led_strip_next_tag() {
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    uint32_t hue;
    uint8_t value = round(brightness);
    static uint32_t start_rgb = 0;

    for (int i = 0; i < 3; i++) {
        for (int j = i; j < STRIP_LED_NUMBER; j += 3) {
            // Build RGB values
            hue = j * 360 / STRIP_LED_NUMBER + start_rgb;
            led_strip_hsv2rgb(hue, 100, value, &red, &green, &blue);
            // Write RGB values to strip driver
            ESP_ERROR_CHECK(strip->set_pixel(strip, j, red, green, blue));
        }
        // Flush RGB values to LEDs
        ESP_ERROR_CHECK(strip->refresh(strip, CHASE_SPEED_MS));
        vTaskDelay(pdMS_TO_TICKS(speed_ms));
        strip->clear(strip, speed_ms);
        vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
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
    uint8_t bright = round(brightness);
    static uint32_t start_rgb = 0;
    for(int i = 0; i < STRIP_LED_NUMBER; i++) {
        hue = 360 / STRIP_LED_NUMBER + start_rgb;
        led_strip_hsv2rgb(hue + i, 100, bright, &red, &green, &blue);
        ESP_ERROR_CHECK(strip->set_pixel(strip, i, red, green, blue));
        if(direct) {
            value +=2;
            if(value >= bright)
                direct = false;
        } else {
            value -= 2;
            if(value == 10) {
                direct = true;
            }
        }
        
    }

    ESP_ERROR_CHECK(strip->refresh(strip, CHASE_SPEED_MS));
    vTaskDelay(pdMS_TO_TICKS(speed_ms));
    start_rgb ++;
}

/**
 * 
 */
void led_strip_next_tail() {
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    uint32_t hue;
    static uint32_t start_rgb = 0;
    static uint16_t num = 0;
    uint16_t pos = 0;
    hue = 360 / STRIP_LED_NUMBER + start_rgb;
    uint8_t value = round(brightness);

    for(int i = 0; i < STRIP_LED_NUMBER; i++) {
        pos = (i + num) % STRIP_LED_NUMBER;
        if(i >= num && i <= num + tail_len) {
            led_strip_hsv2rgb(hue + i, 100, value, &red, &green, &blue);
            value -= 2;
        } else {
            led_strip_hsv2rgb(hue + i, 0, 0, &red, &green, &blue);
        }
        ESP_ERROR_CHECK(strip->set_pixel(strip, pos, red, green, blue));
    }

    num ++;
    if(num >= STRIP_LED_NUMBER) {
        num = 0;
    }

    start_rgb ++;

    ESP_ERROR_CHECK(strip->refresh(strip, CHASE_SPEED_MS));
    vTaskDelay(pdMS_TO_TICKS(speed_ms));
}

/**
 * 
 */
void led_strip_next_blink() {
    ESP_ERROR_CHECK(strip->refresh(strip, CHASE_SPEED_MS));
    led_strip_refresh_color();
    vTaskDelay(pdMS_TO_TICKS(blink_period[0]));
    strip->clear(strip, CHASE_SPEED_MS);
    vTaskDelay(pdMS_TO_TICKS(blink_period[1]));
}

/**
 * 
 **/
void led_strip_next() {
    switch (led_strip_regime) {
    case LED_STRIP_REGIME_OFF:
        led_strip_set_off();
        break;
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
        vTaskDelay(pdMS_TO_TICKS(CHASE_SPEED_MS));
        break;
    }
}
