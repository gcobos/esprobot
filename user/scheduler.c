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

#include "scheduler.h"

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "math.h"
#include "user_interface.h"

#include "espconn.h"
#include "user_json.h"


LOCAL t_saved_scheduler_context scheduler_context;

LOCAL t_axis_activity axis_data[NUM_ACTUATORS * NUM_AXIS_PER_ACTUATOR];
LOCAL uint16 current_axis = 0;

static volatile os_timer_t scheduler_timer;

void ICACHE_FLASH_ATTR scheduler_init(void)
{
	uint16 i;
	
	actuators_init();
	
    // Disarm timer (just in case)
    os_timer_disarm(&scheduler_timer);

    //Setup timer
    os_timer_setfn(&scheduler_timer, (os_timer_func_t *)scheduler_loop, NULL);

	// Clean activity from the axis
	for (i=0; i<NUM_ACTUATORS * NUM_AXIS_PER_ACTUATOR; i++) {
		axis_data[i].vector = 0;
		axis_data[i].last_execution = 0;
	}

	// Init manually the scheduler's loop
	scheduler_loop(NULL);
}

// Find the next active axis to set the PWM out on and decrease duration vector and update statistics
void ICACHE_FLASH_ATTR scheduler_loop(void *arg)
{
	uint16 axis_id = current_axis;
	uint32 now = system_get_time();
	t_axis_activity *activity;
	uint32 active = 0;

	//os_printf("%u: Check queue starting from: %d\n", now, axis_id);
	do {
		axis_id = (axis_id + 1) % (NUM_ACTUATORS * NUM_AXIS_PER_ACTUATOR);
		activity = &axis_data[axis_id]; 
		// If active and last executed is far enough, active axis and break the loop
		if (activity->vector != 0 && now - activity->last_execution > SAFE_AXIS_INACTIVITY_PERIOD) {
			//os_printf("Cuanto es vector?\n", activity->vector);
			activity->vector = (activity->vector > 0)?activity->vector-1 : activity->vector+1;
			current_axis = axis_id;
			
			// Set the axis on
			active = scheduler_execute_axis_quantum(axis_id, activity->vector);
			//os_printf("%d: Activate axis_id %d for %d\n", now, axis_id, active);
			activity->last_execution = now + (active*1000);	
			break;
		}
	} while (axis_id != current_axis);

	if (!active) {
		actuators_disable_pwm_output();
		//os_printf("NO activity, then set the idle timer\n");
		os_timer_disarm(&scheduler_timer);
		os_timer_arm(&scheduler_timer, IDLE_SCHEDULER_CHECK_PERIOD, 0);
	}
}

void ICACHE_FLASH_ATTR
scheduler_load_context()
{
    spi_flash_read((PRIV_PARAM_START_SEC + PRIV_PARAM_SAVE) * SPI_FLASH_SEC_SIZE,
                               (uint32 *)&scheduler_context, sizeof(t_saved_scheduler_context));
}

void ICACHE_FLASH_ATTR scheduler_save_context()
{
/*	extern struct light_saved_param light_param;

	os_memcpy(light_param.pwm_duty,current_duty,sizeof(light_param.pwm_duty));
	light_param.pwm_period =  pwm_get_period();

#if SAVE_LIGHT_PARAM
	spi_flash_erase_sector(PRIV_PARAM_START_SEC + PRIV_PARAM_SAVE);
	spi_flash_write((PRIV_PARAM_START_SEC + PRIV_PARAM_SAVE) * SPI_FLASH_SEC_SIZE,
	    		(uint32 *)&light_param, sizeof(struct light_saved_param));
#endif
*/
}

bool ICACHE_FLASH_ATTR scheduler_move_axis(uint16 axis_id, sint16 vector)
{

	// vector = 0, turns off the actuator
	if (axis_id < 0 || axis_id > (NUM_ACTUATORS * NUM_AXIS_PER_ACTUATOR)-1) {
		return FALSE;
	}
	axis_data[axis_id].vector += vector;			// Movement is accumulative now

	return TRUE;	
}

uint16 ICACHE_FLASH_ATTR scheduler_execute_axis_quantum(uint16 axis_id, sint16 vector)
{
	t_actuator_config *actuator_config;
	t_axis_config *axis_config;
	uint16 sign = (vector<0)? NEGATIVE:POSITIVE;
	bool duty_changed;

	//os_printf("Inside exec quantum function\n");
	
	actuator_config = actuators_get_config(axis_id);
	axis_config = &actuator_config->axis[axis_id & ACTUATOR_BASE];
	duty_changed = (axis_config->duty[sign] != actuators_get_pwm_duty());
	
	if (!vector || duty_changed) {
		actuators_disable_pwm_output();
		if (duty_changed) {
			actuators_set_pwm_duty(axis_config->duty[sign]);
		}
	}

	if (vector && axis_config->quantum) {
		
		actuators_activate_address(actuator_config->address);
	
		//os_printf("Get address of the actuator given its axis %d = %d\n", axis_id>>1, actuator_config->address);
		//os_printf("Quantum for this axis is %d\n", axis_config->quantum);
		//os_printf("Duty for this axis is %d\n", axis_config->duty[sign]);

		if (duty_changed) {
			os_delay_us(DELAY_FOR_DAC_TO_SET);
		}
		actuators_enable_pwm_output();

		// Call again to the scheduler after finishing the quantum for this axis
		os_printf("%u: Running quantum (%dms) for axis_id: %d, vector: %d\n", system_get_time(), axis_config->quantum, axis_id, vector);
		os_timer_disarm(&scheduler_timer);
		os_timer_arm(&scheduler_timer, axis_config->quantum, 0);
	} else {
		return 0;
	}
	
	return axis_config->quantum;
}
