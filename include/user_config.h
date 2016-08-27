#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define ESP_PLATFORM        1

#define USE_OPTIMIZE_PRINTF

#define PLUG_DEVICE             0
#define LIGHT_DEVICE            0
#define SENSOR_DEVICE			0

//#define SERVER_SSL_ENABLE
//#define CLIENT_SSL_ENABLE
//#define UPGRADE_SSL_ENABLE

#define SSID "AndroidAP"
#define SSID_PASSWORD "testpass123"

#define USE_DNS

#ifdef USE_DNS
#define ESP_DOMAIN      "iot.espressif.cn"
#endif

#define SOFTAP_ENCRYPT

#ifdef SOFTAP_ENCRYPT
#define PASSWORD	"v*%W>L<@i&Nxe!"
#endif

#define SENSOR_DEEP_SLEEP

#define SENSOR_DEEP_SLEEP_TIME    30000000

#if PLUG_DEVICE || LIGHT_DEVICE
#define BEACON_TIMEOUT  150000000
#define BEACON_TIME     50000
#endif

#define AP_CACHE           1

#if AP_CACHE
#define AP_CACHE_NUMBER    5
#endif

#endif

