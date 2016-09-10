#ifndef __ACTUATOR_H__
#define __ACTUATOR_H__

#include "bits.h"
extern "C" {
		#include "os_type.h"
		#include "c_types.h"
		#include "eagle_soc.h"
		#include "ets_sys.h"
		#include "osapi.h"
}

// Maximum number of actuators the robot can handle. It depends on the circuit, and affects to the number of control lines needed
// Every actuator has 2 axis. Every axis has 2 tensors, handled by the sign given to the activation of the axis

#define NUM_ACTUATORS    8

#define DEFAULT_QUANTUM  500	   // Default quantum in ms

#define ACT_PWM_PERIOD	  901
#define ACT_PWM_NUM_CHANNELS	1

// Axis definition for the actuators
#define NUM_AXIS_PER_ACTUATOR	2

// Names for axis sides
#define NEGATIVE	0
#define POSITIVE	1

#define AXIS_BASE   0x00
#define X_AXIS      AXIS_BASE + 0
#define Y_AXIS      AXIS_BASE + 1

#define ACTUATOR_BASE   NBITS2(NUM_AXIS_PER_ACTUATOR)

// Output enable pin, to disable PWM while changing values
#define ACT_OUTPUT_ENABLE   0

// PWM and wiring to control the actuators
#define ACT_PWM_OUTPUT      15

// Define addressing pins to select the output for PWM
#define ACT_CONTROL_ADDRESS_A 14	  // GPIO14
#define ACT_CONTROL_ADDRESS_B 13    // GPIO13
#define ACT_CONTROL_ADDRESS_C 12		// GPIO12

typedef struct {
    int  quantum;	// Default quantum for this axis
	  int  duty[2];	// PWM duty (index negative=0, positive=1) for this axis
} t_axis_config;

typedef struct  {
    uint16_t   address;        // Remaps address for this actuator (0 to NUM_ACTUATORS-1)
    t_axis_config   axis[NUM_AXIS_PER_ACTUATOR];
} t_actuator_config;

void ICACHE_FLASH_ATTR actuators_init(void);

// TODO: Tools to configure actuators

void ICACHE_FLASH_ATTR actuators_enable_pwm_output(void);
void ICACHE_FLASH_ATTR actuators_disable_pwm_output(void);
void ICACHE_FLASH_ATTR actuators_set_pwm_duty(uint16_t duty);
uint16_t ICACHE_FLASH_ATTR actuators_get_pwm_duty(void);

t_actuator_config* ICACHE_FLASH_ATTR actuators_get_config(uint16_t axis_id);

uint16_t ICACHE_FLASH_ATTR actuators_get_address(uint16_t actuator_id);

bool ICACHE_FLASH_ATTR actuators_configure_address(uint16_t actuator_id, uint16_t address);

bool ICACHE_FLASH_ATTR actuators_configure_axis_duty(uint16_t axis_id, uint16_t duty_negative, uint16_t duty_positive);

bool ICACHE_FLASH_ATTR actuators_configure_axis_quantum(uint16_t axis_id, uint16_t quantum);

bool ICACHE_FLASH_ATTR actuators_activate_address(uint16_t address);

#endif
