#pragma once 

#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"

/* Include Guard
#ifndef __TIMER_GROUP__
#define __TIMER_GROUP__
*/

#define TIMER_DIVIDER         (16)  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / (TIMER_DIVIDER * 1000))  // convert counter value to useconds

typedef struct {
    int timer_group;
    int timer_idx;
    int alarm_interval;
    bool auto_reload;
} timer_info_t;

/**
 * @brief A sample structure to pass events from the timer ISR to task
 *
 */
typedef struct {
    timer_info_t info;
    uint64_t timer_counter_value;
} timer_event_t;

bool init_tg_timer(int group, int timer, bool auto_reload, int timer_interval_usec);
bool next_tick();

/*
#endif  //__TIMER_GROUP__
*/
