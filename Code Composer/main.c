
/****************************** Includes **********************************/
#include <F28x_Project.h>
#include <stdint.h>
#include <motor.h>
#include <stdio.h>
#include <stdlib.h>
#include "driverlib.h"
#include "led_blade.h"
#include <stdbool.h>
#include <ftdi_spi.h>
#include "codec.h"
/****************************** Defines ***********************************/
#define REED_GPIO 16
/******************************* Enums ************************************/


/****************************** Externs ***********************************/

extern codec_board_t codec;
/************************ Function Declarations ***************************/
void timer1_init(void (*timer1_isr)(void));
#ifdef _FLASH
__attribute__((ramfunc))
#endif
void reed_switch_init(void (*pb_isr)(void));
#ifdef _FLASH
__attribute__((ramfunc))
#endif
int  main_fast();
/******************************* ISR's ************************************/
__interrupt void epwm1_isr();
__interrupt void fault_isr();
__interrupt void timer1_isr();
__interrupt void reed_isr();
__interrupt void codec_isr();
/************************ Global Variables ********************************/
//__attribute__((DELAY_US))
//const byte_trans_t pic[NUM_DIVISIONS]= {
//                                    #include "raw_image.h"
//                                   };

volatile byte_trans_t image[NUM_DIVISIONS]; //= {
//#include "raw_image.h"
//};
#pragma DATA_SECTION(image, "ramgs4");
volatile uint16_t counter = 0;

//#pragma DATA_SECTION(pic, "flash");
volatile uint32_t  word_count = 0; //externed in ftdi_spi
volatile uint16_t speed_delta = 30; //add or subtract period value in increments of this value
volatile motor_state_t motor_accel;
volatile uint32_t timer_per = 250000;
volatile int32_t position = -1;
volatile uint16_t num_interrupts = 0;
volatile bool write_led = false;
volatile bool flip = false;
volatile bool clear = false;
volatile bool skip = true;
volatile int32_t revs = 0;
volatile int32_t base_pos = 0;
volatile bool new_image = false;
volatile uint16_t cs_stat = 1;
volatile state_t mode = visual;
uint16_t rightVol = 0;
uint16_t leftVol = 0;
/**************************** Main Program ********************************/

int main(void)
{
    DisableDog();
    InitSysCtrl(); //run at 200 MHz

    main_fast();
}

#ifdef _FLASH
__attribute__((ramfunc))
#endif
int main_fast()
{
    Interrupt_initModule(); //clear all PIE registers
    Interrupt_initVectorTable(); //clear the PIE vector table
    init_usb_spi();
    Interrupt_enableMaster(); //uncomment to globally enable interrupts

    rx_image_data(); //blocking function
    word_count = 0;

    motor_init(); //initialize and enable driver to control motor
    init_epwm1_interrupt(&epwm1_isr); //enable interrupt for EPWM1
    enable_fault_int(&fault_isr);
    timer1_init(&timer1_isr); //10 ms overflow rate
     //initialize push button interrupt to start/stop motor
     //reed_switch_init(&pb_isr);

     position = 0;
     motor_accel = speed_up;

     GPIO_setPinConfig(GPIO_83_GPIO83);
     GPIO_setPinConfig(GPIO_84_GPIO84);
     GPIO_setPinConfig(GPIO_85_GPIO85);

     GPIO_setDirectionMode(LED_CLK, GPIO_DIR_MODE_OUT);
     GPIO_setDirectionMode(LED_D0, GPIO_DIR_MODE_OUT);
     GPIO_setDirectionMode(LED_D1, GPIO_DIR_MODE_OUT);
     //initialize timer counter to speed up/slow down motor (controls pwm frequency)
     EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP_DOWN);
     CPUTimer_startTimer(CPUTIMER1_BASE);


     codec.sound_init(&codec,SR48);
     Interrupt_register(INT_MCBSPB_RX, &codec_isr);
     //receive interrupt already enabled in InitAIC23 function call
     //Interrupt_enable(INT_MCBSPB_RX);
     while (1)
     {

         if(GPIO_readPin(DSP_CS_GPIO) == 0)
         {
             cs_stat = 0;
         }

         if(!cs_stat && GPIO_readPin(DSP_CS_GPIO) == 1)
         {
             if(mode == audio)
             {
                 Interrupt_enable(INT_MCBSPB_RX);
             }
             else
             {
                 Interrupt_disable(INT_MCBSPB_RX);
             }
             word_count = 0; //reset index in array which receives image through SPI
             cs_stat = 1;
         }

         if(write_led)
         {
             write_led = false;
             //output data to LEDs
             byte_trans_t data = image[position];

             if(mode == visual)
             {
                 if(flip)
                 {
                     sendWordFlippedTransmission(data);
                 }
                 else
                 {
                     sendWordTransmission(data);
                 }
             }
             else if(mode == audio)
             {
                 sendMagnitude(leftVol, 0x1F0000, rightVol, 0x00001F);
                 for (int ii = 0; ii <= 10; ii++) {
                     DELAY_US(1000);
                 }
             }

         }
     }

}

#ifdef _FLASH
__attribute__((ramfunc))
#endif
__interrupt void epwm1_isr()
{
    num_interrupts++;
    if(num_interrupts >= 4)
    {
        write_led = true;
        num_interrupts = 0;
        position+=2; //full step has occured
        if(position > 200)
        {
            flip = !flip;
            position = base_pos;
        }
        if(revs >= 400)
        {
            if(base_pos == 0)
            {
                base_pos = 1;
            }
            else if(base_pos == 1)
            {
                base_pos = 0;
            }
        }

    }

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3); //acknowledge the interrupt group
    EPWM_clearEventTriggerInterruptFlag(EPWM1_BASE); //clear interrupt flag for EPWM module
}

#ifdef _FLASH
__attribute__((ramfunc))
#endif
__interrupt void fault_isr()
{
    //fault occurred, read status register of DRV
    uint16_t fault = read_drv(STATUS_REG);
    if((fault & UVLO) == UVLO)
    {
        write_drv(STATUS_REG, (~UVLO));
    }
    if((fault & AOCP) == AOCP)
    {
        write_drv(STATUS_REG, (~AOCP));
    }
    if((fault & BOCP) ==  BOCP)
    {
        write_drv(STATUS_REG, (~BOCP));
    }
    //group 1, int 4 for XINT1
    PieCtrlRegs.PIEIFR1.bit.INTx4 = 0;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1); //acknowledge the interrupt group
}
uint32_t time_on = 5000; //should delay for 4 seconds
volatile uint32_t time = 0;
volatile int increase = false;
volatile uint16_t curr_period;
volatile uint16_t switched_prescaler = 0;
//uint16_t stop = 0;

#ifdef _FLASH
__attribute__((ramfunc))
#endif
__interrupt void timer1_isr()
{
    if(motor_accel == speed_up)
    {
        uint16_t curr_period = EPWM_getTimeBasePeriod(EPWM1_BASE);
        if(!increase && curr_period < 5000)
        {
            increase = 1;
            CPUTimer_setPeriod(CPUTIMER1_BASE, timer_per << 1); //increase period of this overflow, start increasing slower
            speed_delta = 4;
        }
        if(curr_period > (910))
        {
            uint16_t new_period = curr_period - speed_delta;
            EPWM_setTimeBasePeriod(EPWM1_BASE, new_period);
            EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, new_period>>1); //set CMPA to be half the period
        }
        else
        {
            motor_accel = constant;
        }
    }
    else if(motor_accel == slow_down)
    {
        uint16_t curr_period = EPWM_getTimeBasePeriod(EPWM1_BASE);
        if(curr_period <= 0xFFE0) //keep slowing down
        {
            EPWM_setTimeBasePeriod(EPWM1_BASE, EPWM_getTimeBasePeriod(EPWM1_BASE) + speed_delta);
        }
        else
        {
            if(!switched_prescaler)
            {
                EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_2);
                EPWM_setTimeBasePeriod(EPWM1_BASE, 0xFFF0 >> 1);
                switched_prescaler = 1;
            }
            else{
                write_drv(CTRL_REG, 0x0000); //disable motor
                EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);

            }

        }
    }
    else //constant
    {
        time++;
        if(time > time_on)
        {
            speed_delta = 20;
            motor_accel = slow_down;
        }
        //remain constant
    }
}
#ifdef _FLASH
__attribute__((ramfunc))
#endif
__interrupt void codec_isr()
{
    //in DSP mode, send left channel data then right channel
    codec.rx_audio(&codec); //update internal member variables
    rightVol = (abs(codec.right_data_rx)/60);
    leftVol = (abs(codec.left_data_rx)/60);
    codec.left_data_tx = codec.left_data_rx;
    codec.right_data_tx = codec.right_data_rx;


    codec.tx_audio(&codec);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP6); //acknowledge PIE group for codec interrupt
}



#ifdef _FLASH
__attribute__((ramfunc))
#endif
__interrupt void reed_isr()
{
    position = 0;
    motor_accel = speed_up;

    //group 1, int 5 for XINT2
    PieCtrlRegs.PIEIFR1.bit.INTx5 = 0;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1); //acknowledge the interrupt group
}
#ifdef _FLASH
__attribute__((ramfunc))
#endif
void timer1_init(void (*timer1_isr)(void))
{
    CPUTimer_setPeriod(CPUTIMER1_BASE, timer_per); //1,000,000 required for 10 ms interrupt rate
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE); //reload count registers with period value
    CPUTimer_setPreScaler(CPUTIMER1_BASE, CPUTIMER_CLOCK_PRESCALER_1); //currently divides by 1
    CPUTimer_stopTimer(CPUTIMER1_BASE);

    Interrupt_register(INT_TIMER1, timer1_isr);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    Interrupt_enable(INT_TIMER1);

}

#ifdef _FLASH
__attribute__((ramfunc))
#endif
void reed_switch_init(void (*reed_isr)(void))
{
    //gpio 16 => header 33 for push button
    GPIO_setPinConfig(GPIO_16_GPIO16);
    GPIO_setDirectionMode(REED_GPIO, GPIO_DIR_MODE_IN);

    GPIO_setQualificationMode(REED_GPIO, GPIO_QUAL_ASYNC);
    XBAR_setInputPin(XBAR_INPUT5, REED_GPIO); //xbar input 5 is XINT2
    GPIO_setInterruptPin(REED_GPIO, GPIO_INT_XINT2);
    GPIO_setInterruptType(GPIO_INT_XINT2, GPIO_INT_TYPE_FALLING_EDGE); //fault is active low signal
    Interrupt_register(INT_XINT2, reed_isr);
    GPIO_enableInterrupt(GPIO_INT_XINT2);
    Interrupt_enable(INT_XINT2); //enable interrupt
}

