/*
 * initAIC23.c
 */

#include <stdint.h>
#include <F28x_Project.h>
#include "AIC23.h"

/***************** Defines ***************/
//#define SmallDelay() for(volatile long  i = 0; i < 500000; i++) -> MOVED TO AIC23.h
/***************** Defines ***************/
#define CodecSPI_CLK_PULS {EALLOW; GpioDataRegs.GPASET.bit.GPIO18 = 1; GpioDataRegs.GPACLEAR.bit.GPIO18 = 1;}
#define CodecSPI_CS_LOW {EALLOW; GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;}
#define CodecSPI_CS_HIGH {EALLOW; GpioDataRegs.GPASET.bit.GPIO19 = 1;}


/***************** User Functions *****************/
void InitMcBSPb();
void InitSPIA();
void InitAIC23();
void SpiTransmit(uint16_t data);

void InitBigBangedCodecSPI();
void BitBangedCodecSpiTransmit(Uint16 data);



/***************** User Functions *****************/

void InitAIC23(uint32_t SRMODE)
{
    SmallDelay();
    uint16_t command;
    command = reset();
    BitBangedCodecSpiTransmit (command);
    SmallDelay();
    command = softpowerdown();       // Power down everything except device and clocks
    BitBangedCodecSpiTransmit (command);
    SmallDelay();
    command = linput_volctl(LIV);    // Unmute left line input and maintain default volume
    BitBangedCodecSpiTransmit (command);
    SmallDelay();
    command = rinput_volctl(RIV);    // Unmute right line input and maintain default volume
    BitBangedCodecSpiTransmit (command);
    SmallDelay();
    command = lhp_volctl(LHV);       // Left headphone volume control
    BitBangedCodecSpiTransmit (command);
    SmallDelay();
    command = rhp_volctl(RHV);       // Right headphone volume control
    BitBangedCodecSpiTransmit (command);
    SmallDelay();
    command = nomicaaudpath();      // Turn on DAC, mute mic
    BitBangedCodecSpiTransmit (command);
    SmallDelay();
    command = digaudiopath();       // Disable DAC mute, add de-emph
    BitBangedCodecSpiTransmit (command);
    SmallDelay();

    // I2S
    command = I2Sdigaudinterface(); // AIC23 master mode, I2S mode,32-bit data, LRP=1 to match with XDATADLY=1
    BitBangedCodecSpiTransmit (command);
    SmallDelay();
    command = CLKsampleratecontrol (SRMODE);
    BitBangedCodecSpiTransmit (command);
    SmallDelay();

    command = digact();             // Activate digital interface
    BitBangedCodecSpiTransmit (command);
    SmallDelay();
    command = nomicpowerup();      // Turn everything on except Mic.
    BitBangedCodecSpiTransmit (command);

}

void InitMcBSPb()
{
    /* Init McBSPb GPIO Pins */

    //modify the GPxMUX, GPxGMUX, GPxQSEL
    //all pins should be set to asynch qualification

    /*
     * MDXB -> GPIO12
     * MDRB -> GPIO13
     * MCLKRB -> GPIO3
     * MCLKXB -> GPIO14
     * MFSRB -> GPIO1
     * MFSXB -> GPIO15
     */
    EALLOW;
    // MDXB == GPIO12

    GpioCtrlRegs.GPAGMUX1.bit.GPIO12 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 3;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 3;

    // MDRB == GPIO13

    GpioCtrlRegs.GPAGMUX1.bit.GPIO13 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 3;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 3;

    // MFSRB == GPIO1
    EALLOW;

    GpioCtrlRegs.GPAGMUX1.bit.GPIO1 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 3;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO1 = 3;

    // MFSXB == GPIO15

    GpioCtrlRegs.GPAGMUX1.bit.GPIO15 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 3;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;

    // MCLKRB == GPIO3
    EALLOW;

    GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 3;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 3;

    // MCLKXB == GPIO14

    GpioCtrlRegs.GPAGMUX1.bit.GPIO14 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 3;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 3;
    EDIS;

    /* Init McBSPb for I2S mode */
    EALLOW;
    McbspbRegs.SPCR2.all = 0; // Reset FS generator, sample rate generator & transmitter
    McbspbRegs.SPCR1.all = 0; // Reset Receiver, Right justify word
    McbspbRegs.SPCR1.bit.RJUST = 2; // left-justify word in DRR and zero-fill LSBs
    McbspbRegs.MFFINT.all=0x0; // Disable all interrupts
    McbspbRegs.SPCR1.bit.RINTM = 0; // McBSP interrupt flag - RRDY
    McbspbRegs.SPCR2.bit.XINTM = 0; // McBSP interrupt flag - XRDY
    // Clear Receive Control Registers
    McbspbRegs.RCR2.all = 0x0;
    McbspbRegs.RCR1.all = 0x0;
    // Clear Transmit Control Registers
    McbspbRegs.XCR2.all = 0x0;
    McbspbRegs.XCR1.all = 0x0;
    // Set Receive/Transmit to 32-bit operation
    McbspbRegs.RCR2.bit.RWDLEN2 = 0;
    McbspbRegs.RCR1.bit.RWDLEN1 = 5;
    McbspbRegs.XCR2.bit.XWDLEN2 = 0;
    McbspbRegs.XCR1.bit.XWDLEN1 = 5;
    McbspbRegs.RCR2.bit.RPHASE =  0; // Dual-phase frame for receive CHANGED TO single phase
    McbspbRegs.RCR1.bit.RFRLEN1 = 0; // Receive frame length = 1 word in phase 1 CHANGED to two words in phase
    McbspbRegs.RCR2.bit.RFRLEN2 = 0; // Receive frame length = 1 word in phase 2
    McbspbRegs.XCR2.bit.XPHASE =  0; // Dual-phase frame for transmit CHANGED
    McbspbRegs.XCR1.bit.XFRLEN1 = 0; // Transmit frame length = 1 word in phase 1 CHANGED to two words in phase
    McbspbRegs.XCR2.bit.XFRLEN2 = 0; // Transmit frame length = 1 word in phase 2
    // I2S mode: R/XDATDLY = 1 always
    McbspbRegs.RCR2.bit.RDATDLY = 0;
    McbspbRegs.XCR2.bit.XDATDLY = 0; //changed to zero
    // Frame Width = 1 CLKG period, CLKGDV must be 1 as slave
    McbspbRegs.SRGR1.all = 0x0001;
    McbspbRegs.PCR.all=0x0000;
    // Transmit frame synchronization is supplied by an external source via the FSX pin
    McbspbRegs.PCR.bit.FSXM = 0;
    // Receive frame synchronization is supplied by an external source via the FSR pin
    McbspbRegs.PCR.bit.FSRM = 0;
    // Select sample rate generator to be signal on MCLKR pin
    McbspbRegs.PCR.bit.SCLKME = 1;
    McbspbRegs.SRGR2.bit.CLKSM = 0;
    // Receive frame-synchronization pulses are active low - (L-channel first)
    McbspbRegs.PCR.bit.FSRP = 1;
    // Transmit frame-synchronization pulses are active low - (L-channel first)
    McbspbRegs.PCR.bit.FSXP = 1;
    // Receive data is sampled on the rising edge of MCLKR
    McbspbRegs.PCR.bit.CLKRP = 1; //1


    // Transmit data is sampled on the rising edge of CLKX
    McbspbRegs.PCR.bit.CLKXP = 1; //1

    McbspbRegs.SPCR1.bit.RINTM = 0; //interrupt every frame is 2, 0 is every word

    // The transmitter gets its clock signal from MCLKX
    McbspbRegs.PCR.bit.CLKXM = 0;
    // The receiver gets its clock signal from MCLKR
    McbspbRegs.PCR.bit.CLKRM = 0;
    // Enable Receive Interrupt
    McbspbRegs.MFFINT.bit.RINT = 1;
    // Ignore unexpected frame sync
    //McbspbRegs.XCR2.bit.XFIG = 1;
    McbspbRegs.SPCR2.all |=0x00C0; // Frame sync & sample rate generators pulled out of reset
    SmallDelay();
    McbspbRegs.SPCR2.bit.XRST=1; // Enable Transmitter
    McbspbRegs.SPCR1.bit.RRST=1; // Enable Receiver
    EDIS;
}

void InitBigBangedCodecSPI(){
    /*
     * GPIO19 - CS
     * GPIO19 - CS
     *
     * GPIO18 - CLK
     * GPIO18 - SPICLK
     *
     * GPIO16 == SPISIMOA
     */

    EALLOW;

    //enable pullups
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;

    GpioCtrlRegs.GPAGMUX2.bit.GPIO19 = 0;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO18 = 0;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO16 = 0;

    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0;

    GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO18 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO16 = 1;

    GpioDataRegs.GPASET.bit.GPIO19 = 1;
    GpioDataRegs.GPACLEAR.bit.GPIO18 = 1;

    EDIS;
}

void BitBangedCodecSpiTransmit(Uint16 data){
    CodecSPI_CS_LOW;
    /* Transmit 16 bit data */
    //send data out MSB first
    for(Uint16 i = 16; i > 0; i--){
        GpioDataRegs.GPADAT.bit.GPIO16 = (data >> (i-1)) & 1;
        CodecSPI_CLK_PULS;
    }

    CodecSPI_CS_HIGH;
}


void InitSPIA()
{
    /* Init GPIO pins for SPIA */

    //enable pullups for each pin
    //set to asynch qualification
    //configure each mux

    //SPISTEA -> GPIO19
    //SPISIMOA -> GPIO58
    //Changed to Pin 16
    //SPICLKA -> GPIO18

    EALLOW;

    //enable pullups
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO19 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3;

    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO18 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3;

    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO16 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3;


    //asynch qual

    EDIS;

    /* Init SPI peripheral */
    //THIS CURRENTLY USES HIGH SPEED SO IT WON'T WORK
    SpiaRegs.SPICCR.all = 0x5F; //CLKPOL = 0, SOMI = SIMO (loopback), 16 bit characters
    SpiaRegs.SPICTL.all = 0x06; //master mode, enable transmissions
    SpiaRegs.SPIBRR.all = 50; //gives baud rate of approx 850 kHz

    SpiaRegs.SPICCR.bit.SPISWRESET = 1;
    SpiaRegs.SPIPRI.bit.FREE = 1;

}
void SpiTransmit(uint16_t data)
{
    /* Transmit 16 bit data */
    SpiaRegs.SPIDAT = data; //send data to SPI register
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0); //wait until the data has been sent
    Uint16 dummyLoad = SpiaRegs.SPIRXBUF; //reset flag
}
