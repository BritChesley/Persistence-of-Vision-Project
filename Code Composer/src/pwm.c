#include "pwm.h"
#include <F28x_Project.h>
#include <driverlib.h>

/* Name:        init_epwm1
 * Purpose:     Initializes EPWM1A to have period of 167.76 ms, this function does NOT start the timer. Need to call
 *              EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP_DOWN) to start the timer. 50% duty cycle
 *              sets EPWM clock divider to 128, High speed clock divider to 1. Period and CMPA values are shadow loaded
 *              when count = 0. GPIO 0 (header 40) goes high when CMPA is hit when counting up, low when CMPA hit when
 *              counting down
 * Params:      void
 * Returns:     void
 */
void init_epwm1()
{
    GPIO_setPinConfig(GPIO_0_EPWM1A);
    //disable TBCLK
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    uint16_t pwm_period = 0xFFFF>>1; //should be 50 ms overflow rate
    //configure TBCLK
    //TBCLK = EPWMCLK/(highSpeedPrescaler * pre-scaler), where EPWMCLK is SYSCLK / 2 at reset
    EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1); //TBCLK should be 781.25 KHz with prescaler 128
    EPWM_setTimeBasePeriod(EPWM1_BASE, pwm_period);
    EPWM_setPhaseShift(EPWM1_BASE, 0); //no phase shift between EPWMCLK and TBCLK
    EPWM_setTimeBaseCounter(EPWM1_BASE, 0); //set TBCLK count value to 0
    EPWM_selectPeriodLoadEvent(EPWM1_BASE, EPWM_SHADOW_LOAD_MODE_COUNTER_ZERO); //period value will be updated when counter reaches zero
    EPWM_setCounterCompareShadowLoadMode(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO); //CMPA value will be updated when counter set to zero

    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE); //up down mode, pretty sure this will start the timer, keep timer OFF
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, pwm_period>>1); //set CMPA to be half the period
    EPWM_setActionQualifierAction(EPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA); //when counting up, timer
    EPWM_setActionQualifierAction(EPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    //EPWM_setInterruptEventCount(EPWM1_BASE, 8); //interrupt once out of every 8 interrupt signals. max val is 15, so chose something 32 is
    //enable TBCLK
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}


/* Name:        init_epwm1_interrupt
 * Purpose:     Initializes interrupt when CMPA is hit when counting up. Assigns epwm1_isr to the interrupt vector
 * Params:      void
 * Returns:     void
 */
void init_epwm1_interrupt(void (*epwm1_isr)(void))
{
    Interrupt_register(INT_EPWM1, epwm1_isr);
    EPWM_setInterruptSource(EPWM1_BASE, EPWM_INT_TBCTR_U_CMPA); //interrupt when count = CMPA
    EPWM_enableInterrupt(EPWM1_BASE);
    EPWM_setInterruptEventCount(EPWM1_BASE, 8); //interrupt EVERY time CMPA = count value
    Interrupt_enable(INT_EPWM1);
}
