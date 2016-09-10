
#ifndef __WEBSERVICE_H__
#define __WEBSERVICE_H__

#include "scheduler.h"
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "IPAddress.h"

void ICACHE_FLASH_ATTR webservice_init(IPAddress ip);

void ICACHE_FLASH_ATTR webservice_handle(void);

#endif
