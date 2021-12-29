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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <cmocka.h>
#include "combase.h"
#include <xl4unibase/unibase_binding.h>

#define UB_LOGCAT 2

static int num_timer_test=50;
static int ntimers=0;
static uint64_t timeout=50000;//us
static int timeout_diff=10000;//The diff in usec between two continous timer

#define MAX_LOOP_NSEC ((timeout+num_timer_test*timeout_diff+200000)*1000)
/*
 * Normally tolerant error 500usec is enough. However when a system is weak or CPU load is high
 * the tolerant error need to be a little bigger to make sure the test is always passed.
 * And 2msec should be enough.
*/
#define ERROR_TOLERANT_NSEC 2000000 

struct test_data
{
	uint64_t start_time;
	uint64_t end_time;
	uint64_t timeout;
	cb_xtimer_t *timer;
};

static void test_start_stop(void **state)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;

	timer_man=cb_xtimer_man_create();
	assert_non_null(timer_man);
	timer = cb_xtimer_create(timer_man, NULL, NULL);
	assert_non_null(timer);
	assert_false(cb_xtimer_is_running(timer));
	assert_false(cb_xtimer_is_periodic(timer));
	assert_int_equal(cb_xtimer_start(timer, 10), 0);
	assert_true(cb_xtimer_is_running(timer));
	cb_xtimer_stop(timer);
	assert_false(cb_xtimer_is_running(timer));
	cb_xtimer_delete(timer);
	cb_xtimer_man_delete(timer_man);
}

static void test_time_expire_cb(cb_xtimer_t *timer, void *arg)
{
	struct test_data *data;
	uint64_t diff;

	data=(struct test_data*)arg;
	data->end_time=ub_mt_gettime64();
	diff=data->end_time-data->start_time;
	assert_true(diff > data->timeout*1000u);
	assert_true(diff < data->timeout*1000u+ERROR_TOLERANT_NSEC);
	assert_int_equal(cb_xtimer_remain_timeout(timer), 0);
	assert_false(cb_xtimer_is_running(timer));
	assert_false(cb_xtimer_is_periodic(timer));
	ntimers++;
}

static void test_time_expire(void **state)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;
	int i;
	struct test_data data[num_timer_test];
	uint64_t start, end;

	timer_man=cb_xtimer_man_create();
	assert_non_null(timer_man);

	for(i=0; i<num_timer_test; i++){
		timer = cb_xtimer_create(timer_man, test_time_expire_cb, &data[i]);
		assert_non_null(timer);
		data[i].timer=timer;
		data[i].start_time=ub_mt_gettime64();
		data[i].timeout=timeout+i*timeout_diff;
		assert_int_equal(cb_xtimer_start(timer, data[i].timeout), 0);
	}
	start=ub_mt_gettime64();
	while(1){
		cb_xtimer_man_schedule(timer_man);
		end=ub_mt_gettime64();
		if(end-start>MAX_LOOP_NSEC) break;
	}
	for(i=0; i<num_timer_test; i++){
		assert_false(cb_xtimer_is_running(data[i].timer));
		cb_xtimer_delete(data[i].timer);
	}
	assert_int_equal(ntimers, num_timer_test);
	cb_xtimer_man_delete(timer_man);
}

static void test_time_expire_periodic_option(void **state, xtimer_expirecb_t cb_option)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;
	int i;
	struct test_data data[num_timer_test];
	uint64_t start, end;

	timer_man=cb_xtimer_man_create();
	assert_non_null(timer_man);

	for(i=0; i<num_timer_test; i++){
		timer = cb_xtimer_create(timer_man, cb_option, &data[i]);
		assert_non_null(timer);
		cb_xtimer_set_periodic(timer);
		assert_true(cb_xtimer_is_periodic(timer));
		cb_xtimer_clear_periodic(timer);
		assert_false(cb_xtimer_is_periodic(timer));
		cb_xtimer_set_periodic(timer);
		assert_true(cb_xtimer_is_periodic(timer));

		data[i].timer=timer;
		data[i].start_time=ub_mt_gettime64();
		data[i].timeout=timeout+i*timeout_diff;
		assert_int_equal(cb_xtimer_start(timer, data[i].timeout), 0);
	}
	start=ub_mt_gettime64();
	while(1){
		cb_xtimer_man_schedule(timer_man);
		end=ub_mt_gettime64();
		if(end-start>MAX_LOOP_NSEC) break;
	}
	for(i=0; i<num_timer_test; i++){
		assert_true(cb_xtimer_is_running(data[i].timer));
		cb_xtimer_delete(data[i].timer);
	}
	cb_xtimer_man_delete(timer_man);
}

static void test_time_expire_periodic_cb(cb_xtimer_t *timer, void *arg)
{
	struct test_data *data;
	uint64_t diff;

	data=(struct test_data*)arg;
	data->end_time=ub_mt_gettime64();
	diff=data->end_time-data->start_time;
	assert_in_range(diff, data->timeout*1000u-ERROR_TOLERANT_NSEC,
					data->timeout*1000u+ERROR_TOLERANT_NSEC);
	data->start_time=data->end_time;
}

static void test_time_expire_periodic(void **state)
{
	test_time_expire_periodic_option(state, test_time_expire_periodic_cb);
}

static void test_restart_periodic_timer_incallback_cb(
	cb_xtimer_t *timer, void *arg)
{
	struct test_data *data;
	uint64_t diff;

	assert_false(cb_xtimer_is_running(timer));
	
	data=(struct test_data*)arg;
	data->end_time=ub_mt_gettime64();
	diff=data->end_time-data->start_time;
	assert_in_range(diff, data->timeout*1000u-ERROR_TOLERANT_NSEC,
					data->timeout*1000u+ERROR_TOLERANT_NSEC);
	data->start_time=data->end_time;

	//User can start a timer inside a callback
	assert_int_equal(cb_xtimer_start(timer, data->timeout), 0);
}

static void test_restart_periodic_timer_incallback(void **state)
{
	test_time_expire_periodic_option(
		state, test_restart_periodic_timer_incallback_cb);
}

static void test_time_expire_periodic_mix(void **state)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;
	int i;
	struct test_data data[num_timer_test];
	uint64_t start, end;

	timer_man=cb_xtimer_man_create();
	assert_non_null(timer_man);

	for(i=0; i<num_timer_test; i++){
		timer = cb_xtimer_create(timer_man, test_time_expire_periodic_cb, &data[i]);
		assert_non_null(timer);

		data[i].timer=timer;
		data[i].start_time=ub_mt_gettime64();
		data[i].timeout=timeout+i*timeout_diff;
		if(i%2==0)
			cb_xtimer_set_periodic(timer);
		assert_int_equal(cb_xtimer_start(timer, data[i].timeout), 0);
	}
	start=ub_mt_gettime64();
	while(1){
		cb_xtimer_man_schedule(timer_man);
		end=ub_mt_gettime64();
		if(end-start>MAX_LOOP_NSEC) break;
	}
	for(i=0; i<num_timer_test; i++){
		if(i%2==0)
			assert_true(cb_xtimer_is_running(data[i].timer));
		else
			assert_false(cb_xtimer_is_running(data[i].timer));
		cb_xtimer_delete(data[i].timer);
	}
	cb_xtimer_man_delete(timer_man);
}

static void test_time_expire_periodic_stop_cb(cb_xtimer_t *timer, void *arg)
{
	struct test_data *data;
	uint64_t diff;

	data=(struct test_data*)arg;
	data->end_time=ub_mt_gettime64();
	diff=data->end_time-data->start_time;
	assert_in_range(diff, data->timeout*1000u-ERROR_TOLERANT_NSEC,
					data->timeout*1000u+ERROR_TOLERANT_NSEC);
	data->start_time=data->end_time;
	if(ntimers%2==0)
		cb_xtimer_clear_periodic(timer);
	else
		cb_xtimer_stop(timer);
	ntimers++;
}

static void test_time_expire_periodic_stop(void **state)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;
	int i;
	struct test_data data[num_timer_test];
	uint64_t start, end;

	ntimers=0;

	timer_man=cb_xtimer_man_create();
	assert_non_null(timer_man);

	for(i=0; i<num_timer_test; i++){
		timer = cb_xtimer_create(timer_man, test_time_expire_periodic_stop_cb, &data[i]);
		assert_non_null(timer);

		data[i].timer=timer;
		data[i].start_time=ub_mt_gettime64();
		data[i].timeout=timeout+i*timeout_diff;

		cb_xtimer_set_periodic(timer);
		assert_int_equal(cb_xtimer_start(timer, data[i].timeout), 0);
	}
	start=ub_mt_gettime64();
	while(1){
		cb_xtimer_man_schedule(timer_man);
		end=ub_mt_gettime64();
		if(end-start>MAX_LOOP_NSEC) break;
	}
	for(i=0; i<num_timer_test; i++){
		assert_false(cb_xtimer_is_running(data[i].timer));
		cb_xtimer_delete(data[i].timer);
	}
	cb_xtimer_man_delete(timer_man);
	assert_int_equal(ntimers, num_timer_test);
}

static void test_remain_timeout(void **state)
{
	cb_xtimer_man_t *timer_man;
	cb_xtimer_t *timer;

	timer_man=cb_xtimer_man_create();
	assert_non_null(timer_man);
	timer = cb_xtimer_create(timer_man, NULL, NULL);
	assert_non_null(timer);

	assert_int_equal(cb_xtimer_start(timer, 100), 0);
	assert_true(cb_xtimer_remain_timeout(timer) <= 100);
	assert_true(cb_xtimer_remain_timeout(timer) >= 60);
	usleep(100);
	assert_int_equal(cb_xtimer_remain_timeout(timer), 0);
	cb_xtimer_delete(timer);
	cb_xtimer_man_delete(timer_man);
}

static int setup(void **state)
{
	unibase_init_para_t init_para;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr="4,ubase:45,combase:45";
	unibase_init(&init_para);
	return 0;
}

static int teardown(void **state)
{
	unibase_close();
	return 0;
}

int main(int argc, char **argv)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_start_stop),
		cmocka_unit_test(test_time_expire),
		cmocka_unit_test(test_time_expire_periodic),
		cmocka_unit_test(test_restart_periodic_timer_incallback),
		cmocka_unit_test(test_time_expire_periodic_mix),
		cmocka_unit_test(test_time_expire_periodic_stop),
		cmocka_unit_test(test_remain_timeout),
	};
	return cmocka_run_group_tests(tests, setup, teardown);
}
