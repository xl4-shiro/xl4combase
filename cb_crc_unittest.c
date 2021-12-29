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
 * Copyright (C) 2021 Excelfore Corporation
 * All rights reserved.
 * Author: Nguyen Van Nam(nam.nguyen@excelfore.com)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <cmocka.h>
#include "combase.h"
#include <xl4unibase/unibase_binding.h>

typedef struct {
	char *input;
	uint32_t input_length;
	uint32_t result;
} crcdata_t;

#define DATA_ENTRY_NUM 7
/*
 * Refer to SWS_Crc_00055 of Specification of CRC Routines AUTOSAR CP R20-11.
*/
crcdata_t s_crcdata[DATA_ENTRY_NUM] = {
	{"\x00\x00\x00\x00", 4, 0x2144DF1C},
	{"\xF2\x01\x83", 3, 0x24AB9D77},
	{"\x0F\xAA\x00\x55", 4, 0xB6C9B287},
	{"\x00\xFF\x55\x11", 4, 0x32A06212},
	{"\x33\x22\x55\xAA\xBB\xCC\xDD\xEE\xFF", 9, 0xB0AE863D},
	{"\x92\x6B\x55", 3, 0x9CDEA29B},
	{"\xFF\xFF\xFF\xFF", 4, 0xFFFFFFFF},
};

static void test_crc32_calculate(void **state)
{
	int i;

	assert_int_equal(cb_crc32_calculate(NULL, 0), 0);
	assert_int_equal(cb_crc32_calculate((uint8_t*)0xabcde, 0), 0);
	
	for (i = 0; i < DATA_ENTRY_NUM; i++) {
		assert_int_equal(
			cb_crc32_calculate((uint8_t *)s_crcdata[i].input, s_crcdata[i].input_length),
			s_crcdata[i].result);
	}
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

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_crc32_calculate),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
