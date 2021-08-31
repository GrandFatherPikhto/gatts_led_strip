/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "common.h"

/**
 * Точка входа в программу
 */
void app_main(void) {
    init_gatt();
    init_strip();
    // init_tg_timer(TIMER_GROUP_0, TIMER_0, true, 100);
    while(true) {
        led_strip_next();
    } 
}
