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
 * @defgroup crc crc functions 
 * @{
 * @file cb_crc.h
 * @copyright Copyright (C) 2021 Excelfore Corporation
 * @author Nguyen Van Nam(nam.nguyen@excelfore.com)
 *
 * @brief crc implementation 
 */

#ifndef __CB_CRC_H_
#define __CB_CRC_H_

#include <stdint.h>

/**
 * @brief The function implement the CRC32 calculation based on
 * the IEEE-802.3 CRC32 Ethernet Standard.
 *
 * Polynomial: 04C11DB7h <br>
 * Initial value: FFFFFFFFh <br>
 * Input data reflected: Yes <br>
 * Result data reflected: Yes <br>
 * XOR value: FFFFFFFFh <br>
 *
 * @param bufptr input data buffer 
 * @param bufsize input data buffer size 
 * @return 0 if bufptr = NULL or bufsize = 0, else return calculated CRC
 */
uint32_t cb_crc32_calculate(uint8_t *bufptr, uint32_t bufsize);

#endif
/** @}*/
