#include "locomotion.h"

/********************************************************************************/
void ICACHE_FLASH_ATTR locomotion_init(IPAddress ip)
{
	scheduler_init();

	webservice_init(ip);

}

void ICACHE_FLASH_ATTR locomotion_handle_request()
{
	webservice_handle();
}
