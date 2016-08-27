#ifndef __USER_IOT_VERSION_H__
#define __USER_IOT_VERSION_H__

#include "user_config.h"

#define IOT_VERSION_MAJOR		1U
#define IOT_VERSION_MINOR		0U
#define IOT_VERSION_REVISION	5U

#define VERSION_NUM   (IOT_VERSION_MAJOR * 1000 + IOT_VERSION_MINOR * 100 + IOT_VERSION_REVISION)

//#define VERSION_TYPE      "b"
#define VERSION_TYPE   	  "v"

#define device_type       45772

#define ONLINE_UPGRADE    0
#define LOCAL_UPGRADE     0
#define ALL_UPGRADE       1
#define NONE_UPGRADE      0

#if ONLINE_UPGRADE
#define UPGRADE_FLAG	"O"
#elif  LOCAL_UPGRADE
#define UPGRADE_FLAG	"l"
#elif  ALL_UPGRADE
#define UPGRADE_FLAG	"a"
#elif NONE_UPGRADE
#define UPGRADE_FLAG	"n"
#endif

#define IOT_VERSION


#endif

