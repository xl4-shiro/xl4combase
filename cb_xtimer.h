/*
 * Excelfore Communication Base Library
 * Copyright (C) 2019 Excelfore Corporation (https://excelfore.com)
 *
 * This file is part of Excelfore-combase.
 *
 * Excelfore-combase is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Excelfore-combase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Excelfore-combase.  If not, see
 * <https://www.gnu.org/licenses/old-licenses/gpl-2.0.html>.
 */
/**
 * @defgroup xtimer xtimer utility functions
 * @{
 * @file cb_xtimer.h
 * @copyright Copyright (C) 2020 Excelfore Corporation
 * @author Nguyen Van Nam(nam.nguyen@excelfore.com)
 *
 * @brief xtimer utility functions
 */

#ifndef __CB_XTIMER_H_
#define __CB_XTIMER_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Data handle of a timer manager, inside of it is private 
 */
typedef struct cb_xtimer_man cb_xtimer_man_t;

/**
 * @brief Data handle of a timer, inside of it is private 
 */
typedef struct cb_xtimer cb_xtimer_t;

/**
 * @brief A callback is called when a timer expired.
 */
typedef void (*xtimer_expirecb_t)(cb_xtimer_t *timer, void *exparg);

/**
 * @brief Create a timer manager.
 *
 * The xtimer is non-thread safe function. 
 * Each thread is expected to own a timer manager and all timers are processed in this thread.
 *
 * @return a timer manager: success; NULL: error 
 */
cb_xtimer_man_t *cb_xtimer_man_create(void);

/**
 * @brief Delete a timer manager.
 * @param xtimer_man timer manager data
 * @return void 
 */
void cb_xtimer_man_delete(cb_xtimer_man_t *xtimer_man);

/**
 * @brief The timer manager consumes expired timers and schedule for next timer to be expired.
 *
 * The function is expected to run in a thread loop. 
 * It returns nearest timeout that expects for next timer to be expired.
 * The thread loop should use this timeout as its wait time for event.
 *
 * @param xtimer_man timer manager data
 * @return >= 0: nearest timeout; -1: error; -2: no running timer
 */
int cb_xtimer_man_schedule(cb_xtimer_man_t *xtimer_man);

/**
 * @brief Get a timeout of nearest timer.
 * @param xtimer_man timer manager data
 * @return >= 0: nearest timeout ; -1: error; -2: no running timer
 */
int cb_xtimer_man_nearest_timeout(cb_xtimer_man_t *xtimer_man);

/**
 * @brief Create a timer. 
 * @param xtimer_man timer manager data
 * @param expirecb expire callback
 * @param exparg expired callback argument
 * @return a timer: success; NULL: error 
 */
cb_xtimer_t *cb_xtimer_create(cb_xtimer_man_t *xtimer_man,
				xtimer_expirecb_t expirecb, void *exparg);

/**
 * @brief Delete a timer.
 * @param xtimer a timer data
 * @return void 
 */
void cb_xtimer_delete(cb_xtimer_t *xtimer);

/**
 * @brief Set a periodic timer flag. 
 *
 * If this flag is set, the timer will be restarted when it expired.
 *
 * @param xtimer a timer data
 * @return void 
 */
void cb_xtimer_set_periodic(cb_xtimer_t *xtimer);

/**
 * @brief Clear periodic timer flag.
 * @param xtimer a timer data
 * @return void 
 */
void cb_xtimer_clear_periodic(cb_xtimer_t *xtimer);

/**
 * @brief Check if a timer is periodic 
 * @param xtimer a timer data
 * @return true: periodic; false: not periodic or error 
 */
bool cb_xtimer_is_periodic(cb_xtimer_t *xtimer);

/**
 * @brief Get remaining timeout of a running timer.
 * @param xtimer a timer data
 * @return >= 0: remaining timeout; -1: error 
 */
int cb_xtimer_remain_timeout(cb_xtimer_t *xtimer);

/**
 * @brief Start a timer.
 * @param xtimer a timer data
 * @param timeout_us timeout value in microsec 
 * @return 0: success; -1: error; -2: timer is running
 */
int cb_xtimer_start(cb_xtimer_t *xtimer, uint32_t timeout_us);

/**
 * @brief Stop a running timer, the periodic flag is cleared as well.
 * @param xtimer a timer data
 * @return void 
 */
void cb_xtimer_stop(cb_xtimer_t *xtimer);

/**
 * @brief Check if a timer is running.
 * @param xtimer a timer data
 * @return true: running; false: stopped or error 
 */
bool cb_xtimer_is_running(cb_xtimer_t *xtimer);

#endif /*__CB_XTIMER_H_*/

/** @}*/
