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
/*
 * cb_xtimer.c
 * Copyright (C) 2020 Excelfore Corporation
 * Author: Nguyen Van Nam(nam.nguyen@excelfore.com)
 */
#include "cb_xtimer.h"
#include "combase_private.h"
#include <stdio.h>

struct cb_xtimer_man {
	struct ub_list running_timers;	
};

struct cb_xtimer {
	struct ub_list_node timer_node;
	cb_xtimer_man_t *xtimer_man;
	uint64_t abstime_nsec;
	uint32_t timeout_us;
	bool is_periodic;
	bool is_running;
	void *exparg;
	xtimer_expirecb_t expirecb;
};

static int add_timer_abstime(cb_xtimer_man_t *xtimer_man,
				cb_xtimer_t *xtimer, uint64_t abstime_nsec);

int cb_xtimer_man_schedule(cb_xtimer_man_t *xtimer_man)
{
	struct ub_list expired_timers = UB_LIST_INIT;
	uint64_t cts64 = ub_mt_gettime64();
	cb_xtimer_t *xtimer;
	
	if(!xtimer_man) return -1;
	
	while(!ub_list_isempty(&xtimer_man->running_timers)){
		xtimer=(cb_xtimer_t*)ub_list_head(&xtimer_man->running_timers);
		if(xtimer->abstime_nsec > cts64) break;
		ub_list_unlink(&xtimer_man->running_timers, (struct ub_list_node *)xtimer);
		ub_list_append(&expired_timers, (struct ub_list_node *)xtimer);
	}

	while(!ub_list_isempty(&expired_timers)){
		xtimer=(cb_xtimer_t*)ub_list_head(&expired_timers);
		ub_list_unlink(&expired_timers, (struct ub_list_node *)xtimer);
		xtimer->is_running = false;
		if(xtimer->expirecb)
			xtimer->expirecb(xtimer, xtimer->exparg);
		if(xtimer->is_periodic)
			add_timer_abstime(xtimer_man, xtimer,
				((uint64_t)xtimer->timeout_us*UB_USEC_NS + xtimer->abstime_nsec));
	}

	return cb_xtimer_man_nearest_timeout(xtimer_man);
}

int cb_xtimer_man_nearest_timeout(cb_xtimer_man_t *xtimer_man)
{
	uint64_t cts64 = ub_mt_gettime64();
	cb_xtimer_t *xtimer;
	
	if(!xtimer_man) return -1;
	xtimer = (cb_xtimer_t*)ub_list_head(&xtimer_man->running_timers);
	if(!xtimer) return -2;
	if(xtimer->abstime_nsec <= cts64) return 0;
	return (xtimer->abstime_nsec - cts64)/UB_USEC_NS;
}

void cb_xtimer_clear_periodic(cb_xtimer_t *xtimer)
{
	if(!xtimer) return;
	xtimer->is_periodic = false;
}

void cb_xtimer_set_periodic(cb_xtimer_t *xtimer)
{
	if(!xtimer) return;
	xtimer->is_periodic = true;
}

bool cb_xtimer_is_periodic(cb_xtimer_t *xtimer)
{
	if(!xtimer) return false;
	return xtimer->is_periodic;
}

cb_xtimer_man_t *cb_xtimer_man_create(void)
{
	cb_xtimer_man_t *xtimer_man = malloc(sizeof(*xtimer_man));
	if(!xtimer_man) return NULL;
	memset(xtimer_man, 0, sizeof(*xtimer_man));
	ub_list_init(&xtimer_man->running_timers);
	return xtimer_man;
}

void cb_xtimer_man_delete(cb_xtimer_man_t *xtimer_man)
{
	cb_xtimer_t *xtimer;
	
	if(!xtimer_man) return; 
	while(!ub_list_isempty(&xtimer_man->running_timers)) {
		xtimer=(cb_xtimer_t*)ub_list_head(&xtimer_man->running_timers);
		ub_list_unlink(&xtimer_man->running_timers, (struct ub_list_node*)xtimer);
		xtimer->is_running = false;
		xtimer->xtimer_man = NULL;
	}
	free(xtimer_man);
}

cb_xtimer_t *cb_xtimer_create(cb_xtimer_man_t *xtimer_man,
				xtimer_expirecb_t expirecb, void *exparg)
{
	cb_xtimer_t *xtimer;

	if(!xtimer_man) return NULL;
	xtimer = malloc(sizeof(*xtimer));
	if(!xtimer) return NULL;
	memset(xtimer, 0, sizeof(*xtimer));
	xtimer->xtimer_man = xtimer_man;
	xtimer->expirecb = expirecb;
	xtimer->exparg = exparg;
	
	return xtimer;
}

void cb_xtimer_delete(cb_xtimer_t *xtimer)
{
	if(!xtimer) return;
	if(xtimer->is_running && xtimer->xtimer_man)
		ub_list_unlink(&xtimer->xtimer_man->running_timers,
			(struct ub_list_node*)xtimer);
	free(xtimer);
}

int cb_xtimer_remain_timeout(cb_xtimer_t *xtimer)
{
	uint64_t cts64 = ub_mt_gettime64();

	if(!xtimer) return -1;
	if(!xtimer->is_running || cts64 >= xtimer->abstime_nsec) return 0;
	return (xtimer->abstime_nsec - cts64)/UB_USEC_NS;
}

static int add_timer_abstime(cb_xtimer_man_t *xtimer_man,
				cb_xtimer_t *xtimer, uint64_t abstime_nsec)
{
	cb_xtimer_t *xtimer_tmp;
	struct ub_list_node *timer_node;

	if (xtimer->is_running)
		return -2;
	
	xtimer->is_running = true;
	xtimer->abstime_nsec = abstime_nsec;
	
	xtimer_tmp = (cb_xtimer_t*)ub_list_tail(&xtimer_man->running_timers);

	/* xtimer has largest abstime, so add to the tail */
	if(ub_list_isempty(&xtimer_man->running_timers)
	   || xtimer->abstime_nsec >= xtimer_tmp->abstime_nsec){
		ub_list_append(&xtimer_man->running_timers, (struct ub_list_node *)xtimer);
		return 0;
	}
	/* add in increasing order of abstime_nsec */
	UB_LIST_FOREACH(&xtimer_man->running_timers, timer_node){
		xtimer_tmp = (cb_xtimer_t*)timer_node;
		if(xtimer->abstime_nsec <= xtimer_tmp->abstime_nsec){
			ub_list_insert_before(&xtimer_man->running_timers,
					timer_node, (struct ub_list_node*)xtimer);
			break;
		}
	}
	return 0;
}

int cb_xtimer_start(cb_xtimer_t *xtimer, uint32_t timeout_us)
{
	int res;
	uint64_t abstime_nsec;
	
	if(!xtimer) return -1;

	/* user may call a periodic timer start inside a expirecb */
	if (xtimer->is_periodic && xtimer->abstime_nsec)
		abstime_nsec = xtimer->abstime_nsec + (uint64_t)timeout_us * UB_USEC_NS;
	else
		abstime_nsec = ub_mt_gettime64() + (uint64_t)timeout_us * UB_USEC_NS;
	
	res = add_timer_abstime(xtimer->xtimer_man, xtimer, abstime_nsec);
	if (res != 0)
		return res;

	xtimer->timeout_us = timeout_us;

	return 0;
}

void cb_xtimer_stop(cb_xtimer_t *xtimer)
{		
	if(!xtimer) return;
	if(xtimer->is_running){
		ub_list_unlink(&xtimer->xtimer_man->running_timers,
					&xtimer->timer_node);
		xtimer->is_running = false;
	}
	xtimer->is_periodic = false;
}

bool cb_xtimer_is_running(cb_xtimer_t *xtimer)
{
	if(!xtimer) return false;
	return xtimer->is_running;
}
