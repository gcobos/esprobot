#ifndef __ACTUATOR_H__
#define __ACTUATOR_H__

#include "c_types.h"
#include "bits.h"
#include "spi_flash.h"
#include "eagle_soc.h"
#include "gpio.h"

// Maximum number of actuators the robot can handle. It depends on the circuit, and affects to the number of control lines needed
// Every actuator has 2 axis. Every axis has 2 tensors, handled by the sign given to the activation of the axis

#define NUM_ACTUATORS    8

#define DEFAULT_QUANTUM  500	   // Default quantum in ms

#define PWM_PERIOD	  901
#define PWM_NUM_CHANNELS	1

/* NOTICE !!! ---this is for 512KB spi flash.*/
/* You can change to other sector if you use other size spi flash. */
/* Refer to the documentation about OTA support and flash mapping*/
#define PRIV_PARAM_START_SEC	0xFC
#define PRIV_PARAM_SAVE     0

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
#define ACT_OUTPUT_ENABLE   PERIPHS_IO_MUX_GPIO0_U		// GPIO0
#define ACT_OUTPUT_ENABLE_BIT   BIT0
#define ACT_OUTPUT_ENABLE_FUNC  FUNC_GPIO0


// PWM and wiring to control the actuators
#define ACT_PWM_OUT_IO_MUX PERIPHS_IO_MUX_MTDO_U		// GPIO15
#define ACT_PWM_OUT_IO_NUM  15
#define ACT_PWM_OUT_IO_FUNC  FUNC_GPIO15

// Define addressing pins to select the output for PWM
#define ACT_CONTROL_ADDRESS_A PERIPHS_IO_MUX_MTMS_U		// GPIO14
#define ACT_CONTROL_ADDRESS_A_BIT   BIT14
#define ACT_CONTROL_ADDRESS_A_FUNC  FUNC_GPIO14
#define ACT_CONTROL_ADDRESS_B PERIPHS_IO_MUX_MTCK_U		// GPIO13
#define ACT_CONTROL_ADDRESS_B_BIT   BIT13
#define ACT_CONTROL_ADDRESS_B_FUNC  FUNC_GPIO13
#define ACT_CONTROL_ADDRESS_C PERIPHS_IO_MUX_MTDI_U		// GPIO12
#define ACT_CONTROL_ADDRESS_C_BIT   BIT12
#define ACT_CONTROL_ADDRESS_C_FUNC  FUNC_GPIO12

typedef struct {
    uint16  quantum;	// Default quantum for this axis
	uint16  duty[2];	// PWM duty (index negative=0, positive=1) for this axis
} t_axis_config;

typedef struct  {
    uint16   address;        // Remaps address for this actuator (0 to NUM_ACTUATORS-1)
    t_axis_config   axis[NUM_AXIS_PER_ACTUATOR];
} t_actuator_config;


void ICACHE_FLASH_ATTR actuators_init(void);

// TODO: Tools to configure actuators

void ICACHE_FLASH_ATTR actuators_enable_pwm_output(void);
void ICACHE_FLASH_ATTR actuators_disable_pwm_output(void);

uint16 ICACHE_FLASH_ATTR actuators_get_pwm_duty(void);
void ICACHE_FLASH_ATTR actuators_set_pwm_duty(uint16 duty);

t_actuator_config* ICACHE_FLASH_ATTR actuators_get_config(uint16 axis_id);

uint16 ICACHE_FLASH_ATTR actuators_get_address(uint16 actuator_id);

bool ICACHE_FLASH_ATTR actuators_configure_address(uint16 actuator_id, uint16 address);

bool ICACHE_FLASH_ATTR actuators_configure_axis_duty(uint16 axis_id, uint16 duty_negative, uint16 duty_positive);

bool ICACHE_FLASH_ATTR actuators_configure_axis_quantum(uint16 axis_id, uint16 quantum);

bool ICACHE_FLASH_ATTR actuators_activate_address(uint16 address);

#endif

