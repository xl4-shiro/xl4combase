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
 * @defgroup combase general types and functions
 * @{
 * @file combase.h
 * @author Shiro Ninomiya<shiro@excelfore.com>
 * @copyright Copyright (C) 2019 Excelfore Corporation
 *
 * @brief combase general global header
 *
 * This header is intended to be included by application programs which
 * use 'combase' library.  This is the only one header which needs to be
 * included.
 *
 */
#ifndef __COMBASE_H_
#define __COMBASE_H_

#include <xl4unibase/unibase.h>

#define CB_COMBASE_LOGCAT 1

#ifndef COMBASE_NO_ETH
#include "cb_ethernet.h"
#endif

#ifndef COMBASE_NO_INET
#include "cb_inet.h"
#endif

#ifndef COMBASE_NO_THREAD
#include "cb_thread.h"
#endif

#ifndef COMBASE_NO_SHMEM
#include "cb_ipcshmem.h"
#endif

#ifndef COMBASE_NO_IPCSOCK
#include "cb_ipcsock.h"
#endif

#ifndef COMBASE_NO_EVENT
#include "cb_tmevent.h"
#endif

#ifndef COMBASE_NO_XTIMER
#include "cb_xtimer.h"
#endif

#ifndef COMBASE_NO_CRC
#include "cb_crc.h"
#endif

#endif

/** @}*/
