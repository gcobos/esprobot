#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "actuators.h"
#include "osapi.h"
#include "os_type.h"

// Interval in us since the last activation of an axis to be safe to be reactivated (recommended 1000000)
#define SAFE_AXIS_INACTIVITY_PERIOD  500000

// Interval in ms to wait in case there is nothing to do with any of the axis so far (recommended 200)
#define IDLE_SCHEDULER_CHECK_PERIOD  100

// Time in us to wait for the DAC to set (recommended 25000)
#define DELAY_FOR_DAC_TO_SET	25000

typedef struct {
    sint16 vector;        // Quantums pending to execute for this axis (it can be positive or negative)
	uint32 last_execution;
} t_axis_activity;


typedef struct {
	t_axis_activity axis_data[NUM_ACTUATORS * NUM_AXIS_PER_ACTUATOR];
	
} t_saved_scheduler_context;


void ICACHE_FLASH_ATTR scheduler_init(void);

void ICACHE_FLASH_ATTR scheduler_loop(void *arg);

bool ICACHE_FLASH_ATTR scheduler_move_axis(uint16 axis_id, sint16 vector);

uint16 ICACHE_FLASH_ATTR scheduler_execute_axis_quantum(uint16 axis_id, sint16 vector);

#endif

