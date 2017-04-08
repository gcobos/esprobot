#include <Arduino.h>
#include "actuators.h"

LOCAL t_actuator_config actuators_config[NUM_ACTUATORS];

LOCAL uint16_t _pwm_current_duty = 0;

void ICACHE_FLASH_ATTR actuators_init(void) {

	// Configure PWM pin
	pinMode(ACT_PWM_OUTPUT, OUTPUT);

    // Configure output enable pin
    pinMode(ACT_OUTPUT_ENABLE, OUTPUT);

    //Set addressing pins
	pinMode(ACT_CONTROL_ADDRESS_A, OUTPUT);
    pinMode(ACT_CONTROL_ADDRESS_B, OUTPUT);
	pinMode(ACT_CONTROL_ADDRESS_C, OUTPUT);

	// TODO: Set other pins as inputs maybe?

	// Set all outputs LOW
	digitalWrite(ACT_PWM_OUTPUT, LOW);
	digitalWrite(ACT_OUTPUT_ENABLE, HIGH);
	digitalWrite(ACT_CONTROL_ADDRESS_A, LOW);
	digitalWrite(ACT_CONTROL_ADDRESS_B, LOW);
	digitalWrite(ACT_CONTROL_ADDRESS_C, LOW);

	  // Configure PWM output
	uint8_t i;
	for (i=0; i<NUM_ACTUATORS; i++) {
		actuators_config[i].address = i;
		actuators_config[i].axis[X_AXIS].quantum = DEFAULT_QUANTUM;
		actuators_config[i].axis[X_AXIS].duty[NEGATIVE] = 240;
		actuators_config[i].axis[X_AXIS].duty[POSITIVE] = 720;
		actuators_config[i].axis[X_AXIS].inactivity_period = DEFAULT_INACTIVITY_PERIOD;

		actuators_config[i].axis[Y_AXIS].quantum = DEFAULT_QUANTUM;
		actuators_config[i].axis[Y_AXIS].duty[NEGATIVE] = 480;
		actuators_config[i].axis[Y_AXIS].duty[POSITIVE] = 1023;
		actuators_config[i].axis[Y_AXIS].inactivity_period = DEFAULT_INACTIVITY_PERIOD;
	}

	// Disable PWM output
	actuators_disable_pwm_output();

	// Set frequency for PWM
	analogWriteFreq(ACT_PWM_PERIOD);
}


void ICACHE_FLASH_ATTR actuators_enable_pwm_output(void)
{
		//Serial.println("Enable PWM");
		digitalWrite(ACT_OUTPUT_ENABLE, 0);
}


void ICACHE_FLASH_ATTR actuators_disable_pwm_output(void)
{
		//Serial.println("Disable PWM");
		digitalWrite(ACT_OUTPUT_ENABLE, 1);
}


void ICACHE_FLASH_ATTR actuators_set_pwm_duty(uint16_t duty)
{
		_pwm_current_duty = duty;
		analogWrite(ACT_PWM_OUTPUT, duty);
		Serial.printf("Duty set to %d\n", duty);
}

uint16_t ICACHE_FLASH_ATTR actuators_get_pwm_duty(void)
{
	return _pwm_current_duty;
}

t_actuator_config* ICACHE_FLASH_ATTR actuators_get_config(uint16_t axis_id)
{
	return &actuators_config[axis_id>>ACTUATOR_BASE];
}

uint16_t ICACHE_FLASH_ATTR actuators_get_address(uint16_t actuator_id)
{
	return actuators_config[actuator_id].address;
}

bool ICACHE_FLASH_ATTR actuators_configure_address(uint16_t actuator_id, uint16_t address)
{
	return actuators_config[actuator_id].address = address;
}

bool ICACHE_FLASH_ATTR actuators_configure_axis_duty(uint16_t axis_id, uint16_t duty_negative, uint16_t duty_positive)
{
	t_axis_config *axis_config = &(actuators_config[axis_id >> ACTUATOR_BASE].axis[axis_id & ACTUATOR_BASE]);

	axis_config->duty[NEGATIVE] = duty_negative;
	axis_config->duty[POSITIVE] = duty_positive;

	return true;
}

bool ICACHE_FLASH_ATTR actuators_configure_axis_quantum(uint16_t axis_id, uint16_t quantum)
{
	actuators_config[axis_id >> ACTUATOR_BASE].axis[axis_id & ACTUATOR_BASE].quantum = quantum;
	return true;
}

bool ICACHE_FLASH_ATTR actuators_configure_axis_inactivity_period(uint16_t axis_id, uint16_t inactivity_period)
{
	actuators_config[axis_id >> ACTUATOR_BASE].axis[axis_id & ACTUATOR_BASE].inactivity_period = inactivity_period;
	return true;
}

bool ICACHE_FLASH_ATTR actuators_activate_address(uint16_t address)
{
	if (address < 0 || address > NUM_ACTUATORS-1) {
		return false;
	}

	// Set address for the pwm output
	digitalWrite(ACT_CONTROL_ADDRESS_A, address&4);
	digitalWrite(ACT_CONTROL_ADDRESS_B, address&2);
	digitalWrite(ACT_CONTROL_ADDRESS_C, address&1);

	return true;
}
