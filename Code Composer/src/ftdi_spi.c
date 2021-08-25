#include "ftdi_spi.h"
#include <stdbool.h>

extern volatile uint16_t image[];
extern volatile state_t mode;

extern volatile uint32_t  word_count;
volatile bool rx_started = false;
volatile uint16_t num_ims = 0;
volatile uint16_t prev_num_ims = 0;
volatile bool cs_was_high = false;

//__interrupt void  spic_rx_isr();

#ifdef _FLASH
__attribute__((ramfunc))
#endif
__interrupt void  spic_rx_isr();

#ifdef _FLASH
__attribute__((ramfunc))
#endif
void spic_init_gpio()
{
    //GPIO69 is SPISIMOC
    //GPIO70 is SPISOMIC
    //GPIO71 is SPICLKC
    //GPIO72 is SPIC CS
    GPIO_setPinConfig(GPIO_69_SPISIMOC); //configure GPIO69 pin function as SPIC MOSI
    GPIO_setPinConfig(GPIO_70_SPISOMIC); //configure GPIO70 pin funciton as SPIC MISO
    GPIO_setPinConfig(GPIO_71_SPICLKC); //configure GPIO71 pin function as SPIC CLK
    GPIO_setPinConfig(GPIO_72_SPISTEC); //Chip select GPIO72

    GPIO_setQualificationMode(69, GPIO_QUAL_ASYNC);
    GPIO_setQualificationMode(71, GPIO_QUAL_ASYNC);
    GPIO_setQualificationMode(72, GPIO_QUAL_ASYNC);
    GPIO_setPadConfig(69, GPIO_PIN_TYPE_PULLUP);
}
#ifdef _FLASH
__attribute__((ramfunc))
#endif
void spic_init_slave()
{
    spic_init_gpio();
    SPI_disableModule(SPIC_BASE); //disable module first and do configurations
    EALLOW;
    ClkCfgRegs.LOSPCP.bit.LSPCLKDIV = 0; //set LSPCLK = 200MHz

    SPI_enableHighSpeedMode(SPIC_BASE);
    uint32_t lowspeedclk = SysCtl_getLowSpeedClock(10000000);
    SPI_setConfig(SPIC_BASE, lowspeedclk ,SPI_PROT_POL1PHA1,   //rising edge with delay chosen as SPI mode
                  SPI_MODE_SLAVE, 1000000, 16); //15 MHz baud, 8 bit data
    SPI_setSTESignalPolarity(SPIC_BASE,  SPI_STE_ACTIVE_LOW);

    SPI_enableFIFO(SPIC_BASE);
    SPI_resetTxFIFO(SPIC_BASE);
    SPI_resetRxFIFO(SPIC_BASE);
    SPI_setFIFOInterruptLevel(SPIC_BASE, SPI_FIFO_TX0 , //transmit interrupt whenever there is less than or equal to zero words in tx fifo
                              SPI_FIFO_RX1); //interrupt when there is one or more word in the rx fifo


        SPI_clearInterruptStatus(SPIC_BASE, SPI_INT_RXFF_OVERFLOW | SPI_INT_RXFF);
//    SPI_clearInterruptStatus(SPIC_BASE, SPI_INT_RX_DATA_TX_EMPTY | SPI_INT_RX_OVERRUN);
     SPI_enableInterrupt(SPIC_BASE, SPI_INT_RXFF);
//


    SPI_enableModule(SPIC_BASE); //enable SPIB module
}




#ifdef _FLASH
__attribute__((ramfunc))
#endif
void init_usb_spi()
{
    spic_init_slave();
    Interrupt_register(INT_SPIC_RX, &spic_rx_isr); //actually RX fif interrupt
    //SPI_clearInterruptStatus(SPIC_BASE, SPI_INT_RXFF_OVERFLOW | SPI_INT_RXFF);
    Interrupt_enable(INT_SPIC_RX);
}
//this will place data in the externed rx_buffer
#ifdef _FLASH
__attribute__((ramfunc))
#endif
void rx_image_data()
{
    while(GPIO_readPin(DSP_CS_GPIO) == 1); //wait for chip select to be pulled low
    DELAY_US(100);
    while(GPIO_readPin(DSP_CS_GPIO) == 0); //wait while chip select is high

    cs_was_high = true;
    num_ims++;


}
#ifdef _FLASH
__attribute__((ramfunc))
#endif
__interrupt void  spic_rx_isr()
{

    //group 6 channel 9
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP6);

    uint32_t status = SPI_getInterruptStatus(SPIC_BASE);

    if((status & SPI_INT_RXFF_OVERFLOW) == SPI_INT_RXFF_OVERFLOW)
    {
        //missed data
        SPI_clearInterruptStatus(SPIC_BASE, SPI_INT_RXFF_OVERFLOW);
        while(1);
    }
    if((status & SPI_INT_RXFF) == SPI_INT_RXFF)
    {
        uint16_t num_rx = SPI_getRxFIFOStatus(SPIC_BASE);
        if(word_count == 0)
        {
            mode = (state_t)SPI_readDataNonBlocking(SPIC_BASE); //first word should be state variable
            word_count++;
        }

        for(volatile uint16_t i = 0; i < num_rx - 1; i++)
        {
            image[word_count - 1] = SPI_readDataNonBlocking(SPIC_BASE);
            word_count++;
        }

        SPI_clearInterruptStatus(SPIC_BASE, SPI_INT_RXFF);

    }
//    if((status & SPI_INT_RX_OVERRUN) == SPI_INT_RX_OVERRUN)
//    {
//        SPI_clearInterruptStatus(SPIA_BASE, SPI_INT_RX_OVERRUN);
//        while(1); //trap
//    }
//    if((status & SPI_INT_RX_DATA_TX_EMPTY) == SPI_INT_RX_DATA_TX_EMPTY)
//    {
//        image[word_count++] = SPI_readDataNonBlocking(SPIA_BASE) & 0xFF;
//    }


}
