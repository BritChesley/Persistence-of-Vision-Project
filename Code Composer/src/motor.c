
#include <motor.h>
#include <pwm.h>

void spib_gpio_init()
{
    //GPIO22 (header pin 8) will be used as chip select.
    //SPI module B will be used
    //GPIO 63 (header 55) is MOSIB, GPIO 64 (header 54) is MISOB, GPIO65 (header 47) is SPICLKB
    GPIO_setPinConfig(GPIO_24_SPISIMOB); //configure GPIO63 pin function as SPIB MOSI
    GPIO_setPinConfig(GPIO_25_SPISOMIB); //configure GPIO64 pin funciton as SPIB MISO
    GPIO_setPinConfig(GPIO_22_SPICLKB); //configure GPIO65 pin function as SPIB CLK
    GPIO_setPinConfig(GPIO_20_GPIO20); //Chip select is GPIO

    //need pull up on SDATO (MISO)
    GPIO_setPadConfig(25, GPIO_PIN_TYPE_PULLUP);

    //set MISO to have asynchronous qualification period
    GPIO_setQualificationMode(25, GPIO_QUAL_ASYNC);

    //set chip select out as low, then as an output
    GPIO_writePin(CS_DRV, CS_FALSE); //set GPIO22 low (false)
    GPIO_setDirectionMode(20, GPIO_DIR_MODE_OUT); //set GPIO22 as an output

}

void spib_init()
{
    //max clock speed for DRV8711 is 4 MHz, will run at 1 MHz
    //DRV8711 uses an active high chip select? Yuck.
    SPI_disableModule(SPIB_BASE); //disable module first and do configurations
//    EALLOW;
//    ClkCfgRegs.LOSPCP.bit.LSPCLKDIV = 0; //set LSPCLK = 200MHz
    spib_gpio_init(); //initialize GPIO

    //SPI_enableHighSpeedMode(SPIB_BASE);
    uint32_t lowspeedclk = SysCtl_getLowSpeedClock(10000000);
    SPI_setConfig(SPIB_BASE, lowspeedclk, SPI_PROT_POL0PHA1,   //rising edge with delay chosen as SPI mode
                  SPI_MODE_MASTER, 1000000, 16); //1 MHz baud, 16 bit data

    SPI_enableModule(SPIB_BASE); //enable SPIB module

}


uint16_t spib_write(uint16_t data)
{
    EALLOW;
    SpibRegs.SPITXBUF = data; //write a word to the transmit buffer
    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
    return SpibRegs.SPIRXBUF; //clears flag
}
uint16_t spib_read()
{
    return (spib_write(0xFFFF));
}

//address is 3 bits, data is 12 bits, rd/~wr is 1 bit
void write_drv(uint16_t addr, uint16_t data)
{
    addr = addr & 0x0007;
    uint16_t packet = WR_STROBE & ((addr << 12) | (data & 0x0FFF));
    GPIO_writePin(CS_DRV, CS_TRUE);
    spib_write(packet); //write data in proper format
    GPIO_writePin(CS_DRV, CS_FALSE);

}

uint16_t read_drv(uint16_t addr)
{
    addr = addr & 0x0007;
    uint16_t packet = RD_STROBE | (addr << 12); //data doesnt matter
    GPIO_writePin(CS_DRV, CS_TRUE);
    uint16_t data = spib_write(packet) & 0x0FFF; //write data in proper format
    GPIO_writePin(CS_DRV, CS_FALSE);

    return data;
}

//enable falling edge interrupt for fault pin
void enable_fault_int(void (*fault_isr)(void))
{
    //fault pin is GPIO14 (header 74)
    GPIO_setQualificationMode(FAULT_GPIO, GPIO_QUAL_ASYNC);
    XBAR_setInputPin(XBAR_INPUT4, FAULT_GPIO); //xbar input 4 is XINT1
    GPIO_setInterruptPin(FAULT_GPIO, GPIO_INT_XINT1);
    GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE); //fault is active low signal
    Interrupt_register(INT_XINT1, fault_isr);
    GPIO_enableInterrupt(GPIO_INT_XINT1);
    Interrupt_enable(INT_XINT1); //enable interrupt
}

void motor_gpio_init()
{
    //sleep is GPIO67 (header 5), Step is pin GPIO0 (header 40) (will be used as PWM output) , direction is GPIO111  (header 6)
    //reset is GPIO61 (header 19), fault is pin GPIO14 (will have interrupt) (header 74)
    GPIO_setPinConfig(GPIO_7_GPIO7); //sleep
    GPIO_setPinConfig(GPIO_89_GPIO89); //dir
    GPIO_setPinConfig(GPIO_88_GPIO88); //reset
    GPIO_setPinConfig(GPIO_21_GPIO21); //fault
    GPIO_setPinConfig(GPIO_0_GPIO0); //step

    //set outputs to false
    GPIO_writePin(SLEEP_GPIO, 1); //set sleep high, enable device
    GPIO_writePin(DIR_GPIO, 1); //set DIR to 0, not sure which direction this is
    GPIO_writePin(RESET_GPIO, 1); //set reset high, meaning device will be in reset


    //enable pull ups on Fault pin
    GPIO_setPadConfig(FAULT_GPIO, GPIO_PIN_TYPE_PULLUP);
    //set direction of GPIO control pin
    GPIO_setDirectionMode(SLEEP_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(DIR_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(RESET_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(FAULT_GPIO, GPIO_DIR_MODE_IN);

    /*
     * For testing step gpio manually
     */
//    GPIO_setPinConfig(GPIO_0_GPIO0);
//    GPIO_writePin(STEP_GPIO, 0);
//    GPIO_setDirectionMode(STEP_GPIO, GPIO_DIR_MODE_OUT);

    init_epwm1(); //initialize Step pin for epwm mode, doesnt start timer

}

void motor_init()
{
    motor_gpio_init(); //initialize gpio to control DRV8711
    spib_init(); //initialize SPIB
    //delay for a certain number of cycles to allow device to in reset for a little bit
    DELAY_US(1000); //delay for 100 us
    GPIO_writePin(RESET_GPIO, RESET_FALSE); //pull device out of reset
    DELAY_US(1000); //delay for 100 us
    GPIO_writePin(SLEEP_GPIO, SLEEP_FALSE); //wake this bitch up
    DELAY_US(5000); //delay for 5 ms to let device warm up

    //do test to make sure SPI communication is functional
    uint16_t test = read_drv(CTRL_REG);
    if(test != 0b0000110000010000)
    {
        while(1); //trap here, spi communication failed
    }
//    //for testing motors, wont utilize BEMF sensing, turn off internal stall detect (external detect mode).
//    //also change ISENSE to gain of 40
//    write_drv(CTRL_REG, test | 0x80 | 0x0b001100000000 | 0b000000100); //also chooose 1/16 step for debug
//
//    //now change full scale output current via torque register, 20 => max current of .530 AMPS
//    write_drv(TORQUE_REG, 0b000100000000 | 20 );
//    //enable motor
//    write_drv(CTRL_REG, read_drv(CTRL_REG) | 0x01); //keep default configs for now, but enable motor

    //following the quick spin and tuning guidelines
    write_drv(TORQUE_REG, 0x0100 | 10);
    write_drv(OFF_REG, 0x0032);
    write_drv(BLANK_REG, 0x0100);
    write_drv(DECAY_REG, 0x0510);
    write_drv(STALL_REG, 0x0A02);
    write_drv(DRIVE_REG, 0x0000);
    write_drv(CTRL_REG, 0x0C29); //1/16 step, gain of 5
}

