#include "actuators.h"
//#include "pwm.h"
#include "gpio.h"
#include "user_config.h"
#include "osapi.h"

LOCAL t_actuator_config actuators_config[NUM_ACTUATORS];


void ICACHE_FLASH_ATTR actuators_init(void) {

	uint32 io_info[][3] = {
		{ACT_PWM_OUT_IO_MUX, ACT_PWM_OUT_IO_FUNC, ACT_PWM_OUT_IO_NUM}
	};
    uint32 pwm_duty_init[PWM_NUM_CHANNELS] = {0};

	gpio_init();
		
    // Configure output enable pin
    PIN_FUNC_SELECT(ACT_OUTPUT_ENABLE, ACT_OUTPUT_ENABLE_FUNC);

	// Setup PWM out pin
    //PIN_FUNC_SELECT(ACT_PWM_OUT_IO_MUX, ACT_PWM_OUT_IO_MUX_FUNC);
        
    //Set addressing pins
    PIN_FUNC_SELECT(ACT_CONTROL_ADDRESS_A, ACT_CONTROL_ADDRESS_A_FUNC);
	PIN_FUNC_SELECT(ACT_CONTROL_ADDRESS_B, ACT_CONTROL_ADDRESS_B_FUNC);
	PIN_FUNC_SELECT(ACT_CONTROL_ADDRESS_C, ACT_CONTROL_ADDRESS_C_FUNC);
	
	// Set as output and lear all pins
    gpio_output_set(0, BIT0|BIT2|BIT12|BIT13|BIT14|BIT15, BIT0|BIT2|BIT12|BIT13|BIT14|BIT15, 0);

	// Configure PWM output
	uint8 i;
	for (i=0; i<NUM_ACTUATORS; i++) {
		actuators_config[i].address = i;
		actuators_config[i].axis[X_AXIS].quantum = DEFAULT_QUANTUM;
		actuators_config[i].axis[X_AXIS].duty[NEGATIVE] = 5000;
		actuators_config[i].axis[X_AXIS].duty[POSITIVE] = 16000;

		actuators_config[i].axis[Y_AXIS].quantum = DEFAULT_QUANTUM;
		actuators_config[i].axis[Y_AXIS].duty[NEGATIVE] = 11000;
		actuators_config[i].axis[Y_AXIS].duty[POSITIVE] = 22000;
	}

	// Disable PWM output
	actuators_disable_pwm_output();
	
    // INIT PIN PWM OUTPUT
    pwm_init(PWM_PERIOD, pwm_duty_init, PWM_NUM_CHANNELS, io_info);
    
    //os_printf("PWM PERIOD: %d\r\n", PWM_PERIOD);

	// Debug of PWM driver
    set_pwm_debug_en(0);

}


void ICACHE_FLASH_ATTR actuators_enable_pwm_output(void)
{
	gpio_output_set(0, ACT_OUTPUT_ENABLE_BIT, ACT_OUTPUT_ENABLE_BIT, 0);
	//os_printf("Output enable\n");
}


void ICACHE_FLASH_ATTR actuators_disable_pwm_output(void)
{
	gpio_output_set(ACT_OUTPUT_ENABLE_BIT, 0, ACT_OUTPUT_ENABLE_BIT, 0);
	//os_printf("Output disable\n");
}


void ICACHE_FLASH_ATTR actuators_set_pwm_duty(uint16 duty)
{
	pwm_set_duty(duty, 0);	  // On channel 0
	//os_printf("Duty set to: %d\n", duty);
	pwm_start();
}


uint16 ICACHE_FLASH_ATTR actuators_get_pwm_duty(void)
{
	return pwm_get_duty(0); // On channel 0
}


t_actuator_config* ICACHE_FLASH_ATTR actuators_get_config(uint16 axis_id)
{
	return &actuators_config[axis_id>>ACTUATOR_BASE];
}


uint16 ICACHE_FLASH_ATTR actuators_get_address(uint16 actuator_id)
{
	return actuators_config[actuator_id].address;
}


bool ICACHE_FLASH_ATTR actuators_configure_address(uint16 actuator_id, uint16 address)
{
	return actuators_config[actuator_id].address = address;
}


bool ICACHE_FLASH_ATTR actuators_configure_axis_duty(uint16 axis_id, uint16 duty_negative, uint16 duty_positive)
{
	t_axis_config *axis_config = &(actuators_config[axis_id >> ACTUATOR_BASE].axis[axis_id & ACTUATOR_BASE]);

	axis_config->duty[NEGATIVE] = duty_negative;
	axis_config->duty[POSITIVE] = duty_positive;
	
	return TRUE;
}


bool ICACHE_FLASH_ATTR actuators_configure_axis_quantum(uint16 axis_id, uint16 quantum)
{
	actuators_config[axis_id >> ACTUATOR_BASE].axis[axis_id & ACTUATOR_BASE].quantum = quantum;

	return TRUE;
}


bool ICACHE_FLASH_ATTR actuators_activate_address(uint16 address)
{
	if (address < 0 || address > NUM_ACTUATORS-1) {
		return FALSE;
	}
	
	// Set address for the pwm output
	gpio_output_set(((address&4)?ACT_CONTROL_ADDRESS_A_BIT:0)|((address&2)?ACT_CONTROL_ADDRESS_B_BIT:0)|((address&1)?ACT_CONTROL_ADDRESS_C_BIT:0),
	                ((address&4)?0:ACT_CONTROL_ADDRESS_A_BIT)|((address&2)?0:ACT_CONTROL_ADDRESS_B_BIT)|((address&1)?0:ACT_CONTROL_ADDRESS_C_BIT),
	                ACT_CONTROL_ADDRESS_A_BIT|ACT_CONTROL_ADDRESS_B_BIT|ACT_CONTROL_ADDRESS_C_BIT,
	                0);
	return TRUE;
}

