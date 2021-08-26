#include "common.h"

led_strip_t *strip = NULL;
/** */
uint8_t regime     = STRIP_REGIME_OFF;

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
        ESP_LOGE(RMT_TAG, "install WS2812 driver failed");
        return false;
    }
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 10));
    // Show simple rainbow chasing pattern
    ESP_LOGI(RMT_TAG, "LED Rainbow Chase Start");

    set_regime(STRIP_REGIME_OFF);

    return true;
}

/**
 * 
 **/
bool set_regime(uint8_t regime) {
    ESP_LOGI(RMT_TAG, "Режим: %d", regime);
    switch(regime) {
        case STRIP_REGIME_OFF:
            // return set_strip_color({0, 0, 0, 0});
            strip->clear(strip, 5);
            strip->refresh(strip, 10);
            return true;
        case STRIP_REGIME_ALL:
            set_strip_color(color_value);
            strip->refresh(strip, 10);
            return true;
        break;
        case STRIP_REGIME_RUN:
            return true;
        default:
            return false;
    }

    return true;
}

/**
 * Устанавливает цвет свечения ленты светодиодов
 */
bool set_strip_color(const uint8_t color[4]) {
    if(regime == STRIP_REGIME_ALL) {
        // strip->clear(strip, 100);
        // vTaskDelay(pdMS_TO_TICKS(100));
        ESP_LOGI(RMT_TAG, "Светить всеми лампочками");
        for(int i = 0; i < STRIP_LED_NUMBER; i++) {
            ESP_ERROR_CHECK(strip->set_pixel(strip, i, color[2], color[1], color[0]));
        }
        // vTaskDelay(pdMS_TO_TICKS(10));
        ESP_ERROR_CHECK(strip->refresh(strip, 100));
        return true;
    }

    return false;
}

/**
 * 
 **/
void led_strip_next() {
    if(regime == STRIP_REGIME_RUN) {
        // strip->clear(strip, 100);
        static int number = 0;
        int i,j, pos;
        for(i = number, j = 0; j < STRIP_LED_NUMBER; i++, j++) {
            pos = i % STRIP_LED_NUMBER;
            if(j < STRIP_LED_TAIL) {
                ESP_ERROR_CHECK(strip->set_pixel(
                    strip, pos
                    , color_value[2]
                    , color_value[1]
                    , color_value[0]));
            } else {
                ESP_ERROR_CHECK(strip->set_pixel(strip, pos, 0x00, 0x00, 0x0));
            }
        }

        ESP_ERROR_CHECK(strip->refresh(strip, 100));
        number ++;
        if(number >= STRIP_LED_NUMBER) {
            number = 0;
        }
    }
}