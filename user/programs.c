/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_devicefind.c
 *
 * Description: Find your hardware's information while working any mode.
 *
 * Modification history:
 *     2014/3/12, v1.0 create this file.
*******************************************************************************/

#include "programs.h"

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "math.h"
#include "user_interface.h"

#include "espconn.h"
#include "user_json.h"


void ICACHE_FLASH_ATTR programs_init(void)
{
}
