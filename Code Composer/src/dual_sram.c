/**************************************** Includes **************************************/
#include <F28x_Project.h>
#include "dual_sram.h"
#include <stdlib.h>


/**************************************** Defines ***************************************/


/*********************************** Struct Definitions *********************************/
sram_t sram = {
                .init       =      &sram_board_init,
               .write_block =      &sram_write_block_HS,
               .read_block  =      &sram_read_block_HS,
               .read_word   =      &sram_read_word_HS,
               .write_word  =      &sram_write_word_HS
};

/************************************** Functions ***************************************/

/* Name: spib_gpio_init
 * Purpose: This function initializes the necessary gpio for SPIB functionality. CS0 is GPIO66,
 *          GPIO67 is CS1.
 * Params: None
 * Returns: Void
 */
static inline void spib_gpio_init()
{
    //GPIO63 => SPISIMOB =>GPB31, GPIO64 => SPISOMIIB => GPC0, GPIO 65 => SPICLKB, GPC1
    //GPIO66 => CS0 GPC2, GPIO67 => CS1 GPC3

    // GPIO63 => GPGBMUX2[31:30] = 11, GPBMUX2[31:30] = 11
    // GPIO64 , GPIO 65 => GPCGMUX1[3:0] = 1111, GPCMUX1[3:0] = 1111

    EALLOW;
    //enable spi mode for MISO, MOSI, and SCK
    GpioCtrlRegs.GPBGMUX2.all   |= 0xC0000000;
    GpioCtrlRegs.GPBMUX2.all    |= 0xC0000000;

    GpioCtrlRegs.GPCGMUX1.all   |= 0x0000000F;
    GpioCtrlRegs.GPCMUX1.all    |= 0x0000000F;

    //input signals need to be in asynchronous mode (GPCQSEL reg)
    //Set GPIO64 (MISO) to be asynchronous
    GpioCtrlRegs.GPCQSEL1.all   |= 0x00000003; //asynchronous mode

    //set up chip selects as gpio that are outputs and initially high
    GpioCtrlRegs.GPCMUX1.all    &= ~0x00000F0;
    GpioCtrlRegs.GPCGMUX1.all   &= ~0x00000F0;

    GpioDataRegs.GPCSET.all     |= 0x0000000C; //set both chip selects high
    GpioCtrlRegs.GPCPUD.all     |= 0x0000000C;//keep pull ups enabled

    GpioCtrlRegs.GPCDIR.all     |= 0x0000000C; //set chip selects as outputs

}


/* Name: spib_init_regs_16m
 * Purpose: This function sets up SPIB for communication with the dual sram board.
 *          4 wire, 16 bit data mode, master.
 * Params: None
 * Returns: Void
 */
static inline void spib_init_regs_16m()
{
    EALLOW;
    ClkCfgRegs.LOSPCP.bit.LSPCLKDIV = 0; //set LSPCLK = 200MHz

    SpibRegs.SPICCR.bit.SPISWRESET = 0; //force SPI to reset state
    SpibRegs.SPIPRI.bit.FREE = 1;  //run spi when emulation suspended

    //Data is latched on rising edge of serial clock
    //Rising edge with delay mode

    //master or slave mode
    SpibRegs.SPICTL.bit.MASTER_SLAVE = 1; //master mode
    SpibRegs.SPIPRI.bit.TRIWIRE = 0; //4 wire mode

    //set SPICLK polarity and phase
    SpibRegs.SPICCR.bit.CLKPOLARITY = 0; //data output on rising edge
    SpibRegs.SPICTL.bit.CLK_PHASE = 0; //delay SCK by half cycle
    //set baud rate
    SpibRegs.SPIBRR.all = 4; //run SPI at 40 MHz for now.

    //set SPI character length
    SpibRegs.SPICCR.bit.SPICHAR = 0xF; //16 bit word

    //enable high speed mode
    SpibRegs.SPICCR.bit.HS_MODE = 1;
    //enable transmitter and receiver
    SpibRegs.SPICTL.bit.TALK = 1; //enable transmitter

    SpibRegs.SPICCR.bit.SPISWRESET = 1; //force SPI out of reset state
}


/* Name: sram_board_init
 * Purpose: This function calls the two previous initialization functions
 * Params: sram struct
 * Returns: Void
 */
void sram_board_init(sram_t *sram)
{
    //initialize spi
    spib_gpio_init();
    spib_init_regs_16m();
}

/* Name: write_ram_word_HS
 * Purpose: This function is used to write a word to the sram using 16 data mode. This function
 *          does NOT set chip selects low so it can be used by another function to designate which
 *          SRAM to write to.
 * Params: 32 bit byte aligned address, 16 bit data to write to the sram
 * Returns: Void
 */
static inline void write_ram_word_HS(uint32_t addr, uint16_t data)
{
    EALLOW;
    //send out 8 bit instruction with high byte of address
    SpibRegs.SPITXBUF = ((SEQ_WR_INSTR << 8) | (addr >> 16)); //left adjust value
    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
    uint16_t data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage

    //send out lower word of address
    SpibRegs.SPITXBUF = ((uint16_t)addr); //left adjust value
    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage


    //now send data to be written
    SpibRegs.SPITXBUF = (data); //left adjust value
    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
    data_rcv = SpibRegs.SPIRXBUF;
}
/* Name: read_ram_word_HS
 * Purpose: This function is used to read a word from the sram using 16 data mode. This function
 *          does NOT set chip selects low so it can be used by another function to designate which
 *          SRAM to read from.
 * Params: 32 bit byte aligned address to read from
 * Returns: Void
 */
static inline uint16_t read_ram_word_HS(uint32_t addr)
{
    EALLOW;

    //send out 8 bit instruction with high byte of address
    SpibRegs.SPITXBUF = ((SEQ_RD_INSTR << 8) | (addr >> 16)); //left adjust value
    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
    uint16_t data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage

    //send out lower word of address
    SpibRegs.SPITXBUF = ((uint16_t)addr); //left adjust value
    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage

    //switch to 8 bit mode for 8 dummy cycles
    SpibRegs.SPICCR.bit.SPICHAR = 0x7; //8 bit word

    //8 dummy cycles
    SpibRegs.SPITXBUF = (uint16_t) (0xFF << 8); //left adjust value
    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
    data_rcv = SpibRegs.SPIRXBUF;

    //switch back to 16 bit mode
    SpibRegs.SPICCR.bit.SPICHAR = 0xF; //8 bit word

    //read word
    SpibRegs.SPITXBUF = (0xFFFF); //garbage
    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
    data_rcv = SpibRegs.SPIRXBUF;

    return data_rcv;
}

/* Name: ram_write_word
 * Purpose: This function is used to write a word to the sram using 16 data mode. This assumes
 *          the address has already been sent out, and the sram is continuously able to write data.
 * Params: Data to write to the SRAM
 * Returns: Void
 */
static inline void ram_write_word(uint16_t data)
{
    volatile uint16_t data_rcv;
    SpibRegs.SPITXBUF = data;
    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
    data_rcv = SpibRegs.SPIRXBUF;
}

/* Name: ram_read_word
 * Purpose: This function is used to read a word to the sram using 16 data mode. This assumes
 *          the address has already been sent out, and the sram is continuously able to read data.
 * Params: None
 * Returns: Void
 */
static inline uint16_t ram_read_word()
{
    uint16_t data_rcv;
    SpibRegs.SPITXBUF = 0xffff; //address already sent out, keep reading
    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
    data_rcv = SpibRegs.SPIRXBUF;
    return data_rcv;
}

 /* Name: sram_write_word_HS
  * Purpose: This function is used to write a single word to a generic SRAM. All addressing logic
  *          is taken care off. This function sets chip selects appropriately
  * Params: sram struct, 32 bit word aligned address, and Data to write to the SRAM
  * Returns: Void
  */
void sram_write_word_HS(sram_t *sram, uint32_t word_addr, uint16_t data)
{
    sram->write_buffer = &data;
    uint32_t byte_addr = word_addr << 1;
    EALLOW;
    if(word_addr <= (SRAM1_BOUND_WORD))
    {
        GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1; //set CS low
        write_ram_word_HS(byte_addr, data);
        GpioDataRegs.GPCSET.bit.GPIO66 = 1; //set CS hgh
    }
    else if(word_addr <= SRAM2_END_WORD)
    {
        word_addr = word_addr - (SRAM2_START_WORD);
        byte_addr = word_addr << 1;

        //set chip select low (GPIO67)GPC3
        GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;
        write_ram_word_HS(byte_addr, data);

        //set CS high
        GpioDataRegs.GPCSET.bit.GPIO67 = 1;

    }
}

/* Name: sram_read_word_HS
 * Purpose: This function is used to read a single word from a generic SRAM. All addressing logic
 *          is taken care off. This function sets chip selects
 * Params: sram struct, 32 bit word aligned address
 * Returns: read 16 bit data
 */
uint16_t sram_read_word_HS(sram_t *sram, uint32_t word_addr)
{
    uint16_t data = 0;
    uint32_t byte_addr = word_addr << 1;
    if(word_addr <= (SRAM1_BOUND_WORD))
    {
        EALLOW;
        GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1; //set CS low
        data = read_ram_word_HS(byte_addr);
        GpioDataRegs.GPCSET.bit.GPIO66 = 1; //set CS hgh
    }
    else if(word_addr <= SRAM2_END_WORD)
    {
        EALLOW;
        word_addr = word_addr - (SRAM2_START_WORD);
        byte_addr = word_addr << 1;
        //set chip select low (GPIO67)GPC3
        GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;
        data = read_ram_word_HS(byte_addr);

        //set CS high
        GpioDataRegs.GPCSET.bit.GPIO67 = 1;
    }
    sram->read_buffer = &data;
    return data;
}

/* Name: sram_write_block_HS
 * Purpose: This function is used to write an array of words to a generic SRAM. All addressing logic
 *          is taken care off. This function sets chip selects
 * Params: sram struct, 32 bit word aligned starting address, array of data to write, and number of words to write
 * Returns: Void
 */
void sram_write_block_HS(sram_t * sram, uint32_t start_word_addr, uint16_t *data, uint32_t num_words)
{
    sram->write_buffer = data;
    //each sram is 128K words 17 addr lines, sram1 [0x000, 0x1FFFF] sram2 = [0x20000, 0x3FFFF]
    uint32_t start_byte_addr = start_word_addr << 1;


    int32_t sram1_start = ( start_byte_addr > SRAM1_BOUND_BYTE) ? -1 :  (int32_t)start_byte_addr;
    int32_t sram1_num_words = ((start_byte_addr + num_words<<1) > SRAM1_BOUND_BYTE) ? ((SRAM1_BOUND_BYTE + 1 - start_byte_addr)>>1) : num_words;
    int32_t sram2_end = ( start_byte_addr + (num_words << 1) > SRAM1_BOUND_BYTE) ? (int32_t)(start_byte_addr + (num_words<<1) - SRAM1_BOUND_BYTE - 1) : -1;
    int32_t sram2_num_words = ((start_byte_addr + (uint32_t)num_words<<(1)) > SRAM1_BOUND_BYTE) ? (start_byte_addr + (((uint32_t)(num_words))<<(1)) - SRAM1_BOUND_BYTE - 1)>>1 : 0;
    int32_t sram2_start = (sram2_end - (sram2_num_words<<1)) & (0xFFFFFFFE); //make sure last bit is zero for word alignment
    uint32_t count = 0;

    //if purely in sram2, then the above conditionals dont work
    if(start_byte_addr > SRAM1_BOUND_BYTE)
    {
        sram2_start = start_byte_addr - (SRAM1_BOUND_BYTE + 1);
        sram2_num_words = num_words;
    }

    if(sram1_start != -1 && sram1_num_words > 0)
    {
        EALLOW;
        GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1; //set CS low
        write_ram_word_HS((uint32_t)sram1_start, data[0]); //write first word with exisiting function
        //now write the rest of the bits
        for(uint32_t i = 1; i < sram1_num_words; i++)
        {
                ram_write_word(data[i]); //keep writing data
        }
        GpioDataRegs.GPCSET.bit.GPIO66 = 1; //set CS hgh
        count = sram1_num_words;
    }
    if(sram2_start != -1 && sram2_num_words > 0)
    {
        EALLOW;
        GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1; //set CS low
        write_ram_word_HS((uint32_t)sram2_start, data[count]); //write first word with exisiting function
        //now write the rest of the bits
        for(uint32_t i = 1; i < sram2_num_words; i++)
        {
                ram_write_word(data[count+i]); //keep writing data
        }
        GpioDataRegs.GPCSET.bit.GPIO67 = 1; //set CS hgh
    }
}

/* Name: sram_read_block_HS
 * Purpose: This function is used to read from the SRAM and place all read data into the
 *          the passed buffer. All addressing logic is taken care off. This function sets chip selects
 * Params: sram struct, 32 bit word aligned starting address, buffer to place read data,
 *         and number of words to read
 * Returns: Void
 */
void sram_read_block_HS(sram_t * sram, uint32_t start_word_addr, uint16_t *buffer, uint32_t num_words)
{
    sram->read_buffer = buffer;
    //each sram is 128K words 17 addr lines, sram1 [0x000, 0x1FFFF] sram2 = [0x20000, 0x3FFFF]
    uint32_t start_byte_addr = start_word_addr << 1;


    int32_t sram1_start = ( start_byte_addr > SRAM1_BOUND_BYTE) ? -1 :  (int32_t)start_byte_addr;
    int32_t sram1_num_words = ((start_byte_addr + num_words<<1) > SRAM1_BOUND_BYTE) ? ((SRAM1_BOUND_BYTE + 1 - start_byte_addr)>>1) : num_words;
    int32_t sram2_end = ( start_byte_addr + (num_words << 1) > SRAM1_BOUND_BYTE) ? (int32_t)(start_byte_addr + (num_words<<1) - SRAM1_BOUND_BYTE - 1) : -1;
    int32_t sram2_num_words = ((start_byte_addr + (uint32_t)num_words<<(1)) > SRAM1_BOUND_BYTE) ? (start_byte_addr + (((uint32_t)(num_words))<<(1)) - SRAM1_BOUND_BYTE - 1)>>1 : 0;
    int32_t sram2_start = (sram2_end - (sram2_num_words<<1)) & (0xFFFFFFFE); //make sure last bit is zero for word alignment
    uint32_t count = 0;

    //if purely in sram2, then the above conditionals dont work
    if(start_byte_addr > SRAM1_BOUND_BYTE)
    {
        sram2_start = start_byte_addr - (SRAM1_BOUND_BYTE + 1);
        sram2_num_words = num_words;
    }

    if(sram1_start != -1 && sram1_num_words > 0)
    {
        EALLOW;
        GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1; //set CS low
        buffer[0] = read_ram_word_HS((uint32_t)sram1_start); //write first word with exisiting function
        //now write the rest of the bits
        for(uint32_t i = 1; i < sram1_num_words; i++)
        {
                buffer[i] = ram_read_word(); //keep writing data
        }
        GpioDataRegs.GPCSET.bit.GPIO66 = 1; //set CS hgh
        count = sram1_num_words;
    }
    if(sram2_start != -1 && sram2_num_words > 0)
    {
        EALLOW;
        GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1; //set CS low
        buffer[count] = read_ram_word_HS((uint32_t)sram2_start); //write first word with exisiting function
        //now write the rest of the bits
        for(uint32_t i = 1; i < sram2_num_words; i++)
        {
                buffer[count+i] = ram_read_word(); //keep writing data
        }
        GpioDataRegs.GPCSET.bit.GPIO67 = 1; //set CS hgh
    }
}

/********************************************* Unused Functions that may be useful ****************************************/
//static inline void spib_init_regs()
//{
//    EALLOW;
//    ClkCfgRegs.LOSPCP.bit.LSPCLKDIV = 0; //set LSPCLK = 200MHz
//
//    SpibRegs.SPICCR.bit.SPISWRESET = 0; //force SPI to reset state
//    SpibRegs.SPIPRI.bit.FREE = 1;  //run spi when emulation suspended
//
//    //Data is latched on rising edge of serial clock
//    //Rising edge with delay mode
//
//    //master or slave mode
//    SpibRegs.SPICTL.bit.MASTER_SLAVE = 1; //master mode
//    SpibRegs.SPIPRI.bit.TRIWIRE = 0; //4 wire mode
//
//    //set SPICLK polarity and phase
//    SpibRegs.SPICCR.bit.CLKPOLARITY = 0; //data output on rising edge
//    SpibRegs.SPICTL.bit.CLK_PHASE = 0; //delay SCK by half cycle
//    //set baud rate
//    SpibRegs.SPIBRR.all = 4; //run SPI at 40 MHz for now.
//
//    //set SPI character length
//    SpibRegs.SPICCR.bit.SPICHAR = 0x7; //8 bit word
//
//    //enable high speed mode
//    SpibRegs.SPICCR.bit.HS_MODE = 1;
//    //enable transmitter and receiver
//    SpibRegs.SPICTL.bit.TALK = 1; //enable transmitter
//
//    SpibRegs.SPICCR.bit.SPISWRESET = 1; //force SPI out of reset state
//
//}

//
////write a byte to RAM1
//static inline uint16_t write_ram1_byte(uint32_t addr, uint16_t data)
//{
//    EALLOW;
//    //set chip select low (GPIO66)GPC2
//    GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;
//
//    //send out 8 bit instruction
//    SpibRegs.SPITXBUF = (SEQ_WR_INSTR) << 8; //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    uint16_t data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //send out 24 bit address
//    SpibRegs.SPITXBUF = (addr >> 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (addr); //left adjust value
//
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (addr << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //now send data to be written
//    SpibRegs.SPITXBUF = ((data & 0xFF)<<8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF;
//
//    //set chip select high
//    GpioDataRegs.GPCSET.bit.GPIO66 = 1;
//
//    return data_rcv;
//}
//
//void write_ram1_word(uint32_t word_addr, uint16_t data)
//{
//    uint32_t byte_addr = word_addr << 1;
//    write_ram1_byte(byte_addr, data); //send out low byte
//    write_ram1_byte(byte_addr+1, data >> 8); //send out high byte
//}
//
//static inline uint16_t read_ram1_byte(uint32_t addr)
//{
//    EALLOW;
//    //set chip select low (GPIO66)GPC2
//    GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;
//
//    //send out 8 bit instruction
//    SpibRegs.SPITXBUF = (SEQ_RD_INSTR) << 8; //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    uint16_t data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //send out 24 bit address
//    SpibRegs.SPITXBUF = (addr >> 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (addr); //left adjust value
//
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (addr << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //8 dummy cycles
//    SpibRegs.SPITXBUF = (0x37 << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF;
//
//    //read data
//    SpibRegs.SPITXBUF = (0x44 << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF;
//
//    //set chip select high
//    GpioDataRegs.GPCSET.bit.GPIO66 = 1;
//
//    return data_rcv;
//
//}
//
//uint16_t read_ram1_word(uint32_t word_addr)
//{
//    uint32_t byte_addr = word_addr << 1;
//    uint16_t result = read_ram1_byte(byte_addr); //get low value
//    result |= (read_ram1_byte(byte_addr+1) << 8);
//    return result;
//}
//
////write a byte to RAM2
//static inline uint16_t write_ram2_byte(uint32_t addr, uint16_t data)
//{
//    EALLOW;
//    //set chip select low (GPIO66)GPC2
//    GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;
//
//    //send out 8 bit instruction
//    SpibRegs.SPITXBUF = (SEQ_WR_INSTR) << 8; //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    uint16_t data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //send out 24 bit address
//    SpibRegs.SPITXBUF = (addr >> 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (addr); //left adjust value
//
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (addr << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //now send data to be written
//    SpibRegs.SPITXBUF = ((data & 0xFF)<<8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF;
//
//    //set chip select high
//    GpioDataRegs.GPCSET.bit.GPIO67 = 1;
//
//    return data_rcv;
//}
//
//void write_ram2_word(uint32_t word_addr, uint16_t data)
//{
//    uint32_t byte_addr = word_addr << 1;
//    write_ram2_byte(byte_addr, data); //send out low byte
//    write_ram2_byte(byte_addr+1, data >> 8); //send out high byte
//}
//
//static inline uint16_t read_ram2_byte(uint32_t addr)
//{
//    EALLOW;
//    //set chip select low (GPIO66)GPC2
//    GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;
//
//    //send out 8 bit instruction
//    SpibRegs.SPITXBUF = (SEQ_RD_INSTR) << 8; //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    uint16_t data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //send out 24 bit address
//    SpibRegs.SPITXBUF = (addr >> 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (addr); //left adjust value
//
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (addr << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //8 dummy cycles
//    SpibRegs.SPITXBUF = (0x37 << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF;
//
//    //read data
//    SpibRegs.SPITXBUF = (0x44 << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF;
//
//    //set chip select high
//    GpioDataRegs.GPCSET.bit.GPIO67 = 1;
//
//    return data_rcv;
//
//}
//
//uint16_t read_ram2_word(uint32_t word_addr)
//{
//    uint32_t byte_addr = word_addr << 1;
//    uint16_t result = read_ram2_byte(byte_addr); //get low value
//    result |= (read_ram2_byte(byte_addr+1) << 8);
//    return result;
//}
//
//void write_sram1(uint32_t start_word_addr, uint16_t *data, uint16_t num_words)
//{
//    EALLOW;
//    //set chip select low (GPIO66)GPC2
//    GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;
//
//    //send out 8 bit instruction
//    SpibRegs.SPITXBUF = (SEQ_WR_INSTR) << 8; //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    uint16_t data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //send out 24 bit address
//    SpibRegs.SPITXBUF = (start_word_addr >> 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (start_word_addr); //left adjust value
//
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (start_word_addr << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    uint16_t *data_ptr = data;
//    for(uint16_t i = 0; i < num_words; i++)
//    {
//        //now send data to be written
//        //write low byte
//        SpibRegs.SPITXBUF = ((*data_ptr)<<8); //left adjust value
//        while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//        data_rcv = SpibRegs.SPIRXBUF;
//        //write high byte at next address, auto incremented
//        SpibRegs.SPITXBUF = (*data_ptr); //left adjust value
//        while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//        data_rcv = SpibRegs.SPIRXBUF;
//        data_ptr++;
//    }
//
//
//    //set chip select high
//    GpioDataRegs.GPCSET.bit.GPIO66 = 1;
//
//}
//
//void read_sram1(uint32_t start_word_addr, uint16_t *buffer, uint16_t num_words)
//{
//    EALLOW;
//    //set chip select low (GPIO66)GPC2
//    GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;
//
//    //send out 8 bit instruction
//    SpibRegs.SPITXBUF = (SEQ_RD_INSTR) << 8; //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    uint16_t data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //send out 24 bit address
//    SpibRegs.SPITXBUF = (start_word_addr >> 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (start_word_addr); //left adjust value
//
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (start_word_addr << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //8 dummy cycles
//    SpibRegs.SPITXBUF = (0x37 << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF;
//
//    uint16_t read_data = 0;
//    for(uint16_t i = 0; i < num_words; i++)
//    {
//        //read low byte
//        SpibRegs.SPITXBUF = (0x44 << 8); //left adjust value
//        while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//        read_data = SpibRegs.SPIRXBUF;
//
//        //read high byte
//        SpibRegs.SPITXBUF = (0x44 << 8); //left adjust value
//        while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//        read_data |= (SpibRegs.SPIRXBUF << 8);
//        buffer[i] = read_data;
//    }
//
//    //set chip select high
//    GpioDataRegs.GPCSET.bit.GPIO66 = 1;
//}
//
//
//void write_sram2(uint32_t start_word_addr, uint16_t *data, uint16_t num_words)
//{
//    EALLOW;
//    //set chip select low (GPIO67)GPC3
//    GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;
//
//    //send out 8 bit instruction
//    SpibRegs.SPITXBUF = (SEQ_WR_INSTR) << 8; //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    uint16_t data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //send out 24 bit address
//    SpibRegs.SPITXBUF = (start_word_addr >> 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (start_word_addr); //left adjust value
//
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (start_word_addr << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    uint16_t *data_ptr = data;
//    for(uint16_t i = 0; i < num_words; i++)
//    {
//        //now send data to be written
//        //write low byte
//        SpibRegs.SPITXBUF = ((*data_ptr)<<8); //left adjust value
//        while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//        data_rcv = SpibRegs.SPIRXBUF;
//        //write high byte at next address, auto incremented
//        SpibRegs.SPITXBUF = (*data_ptr); //left adjust value
//        while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//        data_rcv = SpibRegs.SPIRXBUF;
//        data_ptr++;
//    }
//
//
//    //set chip select high
//    GpioDataRegs.GPCSET.bit.GPIO67 = 1;
//
//}
//
//void read_sram2(uint32_t start_word_addr, uint16_t *buffer, uint16_t num_words)
//{
//    EALLOW;
//    //set chip select low (GPIO67)GPC3
//    GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;
//
//    //send out 8 bit instruction
//    SpibRegs.SPITXBUF = (SEQ_RD_INSTR) << 8; //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    uint16_t data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //send out 24 bit address
//    SpibRegs.SPITXBUF = (start_word_addr >> 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (start_word_addr); //left adjust value
//
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    SpibRegs.SPITXBUF = (start_word_addr << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF; //this clears the flag, value in data is garbage
//
//    //8 dummy cycles
//    SpibRegs.SPITXBUF = (0x37 << 8); //left adjust value
//    while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//    data_rcv = SpibRegs.SPIRXBUF;
//
//    uint16_t read_data = 0;
//    for(uint16_t i = 0; i < num_words; i++)
//    {
//        //read low byte
//        SpibRegs.SPITXBUF = (0x44 << 8); //left adjust value
//        while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//        read_data = SpibRegs.SPIRXBUF;
//
//        //read high byte
//        SpibRegs.SPITXBUF = (0x44 << 8); //left adjust value
//        while(!SpibRegs.SPISTS.bit.INT_FLAG); //wait for interrupt flag
//        read_data |= (SpibRegs.SPIRXBUF << 8);
//        buffer[i] = read_data;
//    }
//
//    //set chip select high
//    GpioDataRegs.GPCSET.bit.GPIO67 = 1;
//}

//void sram_write_block(sram_t* sram, uint32_t start_word_addr, uint16_t *data, uint32_t num_words)
//{
//    sram->write_buffer = data;
//    //each sram is 128K words 17 addr lines, sram1 [0x000, 0x1FFFF] sram2 = [0x20000, 0x3FFFF]
//    uint32_t start_byte_addr = start_word_addr << 1;
//
//
//    int32_t sram1_start = ( start_byte_addr > SRAM1_BOUND_BYTE) ? -1 :  (int32_t)start_byte_addr;
//    int32_t sram1_num_words = (num_words) % SRAM_SIZE_WORD;
//    int32_t sram2_start = ( start_byte_addr > SRAM1_BOUND_BYTE) ? (SRAM1_BOUND_BYTE -  (int32_t)start_byte_addr) : -1;
//    int32_t sram2_num_words = SRAM_SIZE_WORD - (num_words);
//    uint16_t *sram2_ptr = &data[sram2_start];
//    if(sram1_start != -1)
//    {
//        write_sram1((uint32_t) sram1_start, data, (uint32_t)sram1_num_words);
//    }
//
//    if(sram2_start != -1 && sram2_num_words > 0)
//    {
//        write_sram2((uint32_t) sram2_start, sram2_ptr, (uint32_t)sram2_num_words);
//    }
//
//}
//
//
//
//void sram_write_word(sram_t *sram, uint32_t word_addr, uint16_t data)
//{
//    if(word_addr <= (SRAM1_BOUND_WORD))
//    {
//        write_ram1_word(word_addr, data);
//    }
//    else if(word_addr <= SRAM2_END_WORD)
//    {
//        word_addr = word_addr - (SRAM2_START_WORD);
//        write_ram2_word(word_addr, data);
//    }
//}
//
//uint16_t sram_read_word(sram_t *sram, uint32_t word_addr)
//{
//    uint16_t result = 0;
//    if(word_addr <= (SRAM1_BOUND_WORD))
//    {
//        result = read_ram1_word(word_addr);
//    }
//    else if(word_addr <= SRAM2_END_WORD)
//    {
//        word_addr = word_addr - (SRAM2_START_WORD);
//        result = read_ram2_word(word_addr);
//    }
//    else
//        return 0x37;
//
//    return result;
//}
