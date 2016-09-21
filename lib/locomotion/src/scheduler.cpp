#include <Arduino.h>
#include "scheduler.h"

LOCAL t_saved_scheduler_context scheduler_context;

LOCAL t_axis_activity axis_data[NUM_ACTUATORS * NUM_AXIS_PER_ACTUATOR];

LOCAL uint16_t current_axis = 0;

static os_timer_t scheduler_timer;

void ICACHE_FLASH_ATTR scheduler_init(void)
{
		uint16_t i;

		actuators_init();

    // Disarm timer (just in case)
    os_timer_disarm(&scheduler_timer);

    //Setup timer
    os_timer_setfn(&scheduler_timer, (os_timer_func_t *)scheduler_loop, 0);

		// Clean activity from the axis
		for (i=0; i<NUM_ACTUATORS * NUM_AXIS_PER_ACTUATOR; i++) {
		  	axis_data[i].vector = 0;
			  axis_data[i].last_execution = 0;
		}

		// Force the first call to the scheduler
		os_timer_arm(&scheduler_timer, IDLE_SCHEDULER_CHECK_PERIOD, 0);
}

// Find the next active axis to set the PWM out on and decrease duration vector and update statistics
void ICACHE_FLASH_ATTR scheduler_loop(void *arg)
{
		uint16_t axis_id = current_axis;
		uint32_t now = micros();
		t_axis_activity *activity;
		t_actuator_config *actuator_config;
		uint32_t active = 0;

		//os_printf("%u: Check queue starting from: %d\n", now, axis_id);
		do {
				axis_id = (axis_id + 1) % (NUM_ACTUATORS * NUM_AXIS_PER_ACTUATOR);
				activity = &axis_data[axis_id];
				actuator_config = actuators_get_config(axis_id);
				// If active and last executed is far enough, active axis and break the loop
				if (activity->vector != 0 && now - activity->last_execution > actuator_config->axis[axis_id & ACTUATOR_BASE].inactivity_period) {
						//os_printf("Cuanto es vector?\n", activity->vector);
						activity->vector = (activity->vector > 0)?activity->vector-1 : activity->vector+1;
						current_axis = axis_id;

						// Set the axis on
						active = scheduler_execute_axis_quantum(axis_id, activity->vector);
						if (active) {
								Serial.print(now);
								Serial.print(" Activate axis_id ");
								Serial.print(axis_id);
								Serial.print(" for ");
								Serial.println(active);
						}
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
		// TODO
}

void ICACHE_FLASH_ATTR scheduler_save_context()
{
		// TODO
}

bool ICACHE_FLASH_ATTR scheduler_move_axis(uint16_t axis_id, sint16 vector)
{
	// vector = 0, turns off the actuator
	if (axis_id < 0 || axis_id > (NUM_ACTUATORS * NUM_AXIS_PER_ACTUATOR)-1) {
		return false;
	}
	Serial.println(" MOVE axis!!! ");
	axis_data[axis_id].vector += vector;			// Movement is accumulative now

	return true;
}

uint16_t ICACHE_FLASH_ATTR scheduler_execute_axis_quantum(uint16_t axis_id, sint16 vector)
{
		t_actuator_config *actuator_config;
		t_axis_config *axis_config;
		uint16_t sign = (vector<0)? NEGATIVE:POSITIVE;
		bool duty_changed;

		//Serial.println("Inside exec quantum function");
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
						//Serial.println("Antes del delay");
						delayMicroseconds(DELAY_FOR_DAC_TO_SET);
						//Serial.println("Despues del delay");
				}

				//Serial.println("Antes de enable output");
				actuators_enable_pwm_output();
				//Serial.println("Despues de enable output");

				// Call again to the scheduler after finishing the quantum for this axis
				//os_printf("%u: Running quantum (%dms) for axis_id: %d, vector: %d\n", micros(), axis_config->quantum, axis_id, vector);
				os_timer_disarm(&scheduler_timer);
				os_timer_arm(&scheduler_timer, axis_config->quantum, 0);
		} else {
				return 0;
		}

		return axis_config->quantum;
}
