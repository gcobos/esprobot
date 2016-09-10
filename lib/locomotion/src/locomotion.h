#ifndef __LOCOMOTION_H__
#define __LOCOMOTION_H__

#include "scheduler.h"
#include "webservice.h"
#include "IPAddress.h"

void ICACHE_FLASH_ATTR locomotion_init(IPAddress ip);

void ICACHE_FLASH_ATTR locomotion_handle_request();

#endif
