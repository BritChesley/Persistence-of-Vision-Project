#ifndef _PWM_H
#define _PWM_H

/* Name:        init_epwm1
 * Purpose:     Initializes EPWM1A to have period of 167.76 ms, this function does NOT start the timer. Need to call
 *              EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP_DOWN) to start the timer. 50% duty cycle
 *              sets EPWM clock divider to 128, High speed clock divider to 1. Period and CMPA values are shadow loaded
 *              when count = 0. GPIO 0 (header 40) goes high when CMPA is hit when counting up, low when CMPA hit when
 *              counting down
 * Params:      void
 * Returns:     void
 */
void init_epwm1();

/* Name:        init_epwm1_interrupt
 * Purpose:     Initializes interrupt when CMPA is hit when counting up. Assigns epwm1_isr to the interrupt vector
 * Params:      void
 * Returns:     void
 */
void init_epwm1_interrupt(void (*epwm1_isr)(void));

#endif
