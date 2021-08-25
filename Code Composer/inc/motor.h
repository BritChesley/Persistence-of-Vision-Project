#ifndef _MOTOR_H
#define _MOTOR_H

#include <F28x_Project.h>
#include <driverlib.h>
#include "pwm.h"

#define CS_DRV 20 //GPIO22 is used as chip select for DRV8711 driver
#define CS_FALSE 0
#define CS_TRUE 1


#define RD_STROBE 0x8000 //15th bit high for read
#define WR_STROBE 0x7FFF //low for wr_strobe

#define STEP_GPIO 0
#define SLEEP_GPIO 7
#define DIR_GPIO 89
#define RESET_GPIO 88
#define FAULT_GPIO 21

#define SLEEP_TRUE 0
#define SLEEP_FALSE 1
#define RESET_TRUE 1
#define RESET_FALSE 0


/***** DRV8711 Register defines ******/
#define CTRL_REG 0x00
#define TORQUE_REG 0x01
#define OFF_REG 0x02
#define BLANK_REG 0x03
#define DECAY_REG 0x04
#define STALL_REG 0x05
#define DRIVE_REG 0x06
#define STATUS_REG 0x07


/****** Status register defines *******/
#define OTS 0x01
#define AOCP    (0x01<<1)
#define BOCP    (0x01<<2)
#define APDF    (0x01<<3)
#define BPDF    (0x01<<4)
#define UVLO    (0x01<<5)
#define STD     (0x01<<6)
#define STDLAT  (0x01<<7)

#define PWM_PERIOD_START 50000
#define PWM_PERIOD_5RPS 12500
#define KHZ_2 25000
#define HZ_2667 18750    //for 400 RPMS (limit of slip ring)
#define KHZ_3 16667
#define KHZ_4 12500
typedef enum{
    speed_up = 0,
    slow_down = 1,
    constant = 2
}motor_state_t;


void motor_gpio_init();
void motor_init();
void enable_fault_int(void (*fault_isr)(void));
void write_drv(uint16_t addr, uint16_t data);
uint16_t read_drv(uint16_t addr);
#endif
