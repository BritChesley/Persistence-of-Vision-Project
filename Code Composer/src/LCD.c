/********************************** Includes **********************************/
#include <F2837xD_Device.h>
#include "LCD.h"
#include <stdlib.h>
/************************ Struct Definition for the LCD ***********************/

/* To use this lcd definition, need an "extern lcd_t lcd" in the main C file.
 * This initialization sets up the member function pointers to point to the proper
 * functions for the current LCD
 */
volatile lcd_t lcd = {
             .init = &lcd_init,
             .send_command = &lcd_send_command,
             .write = &lcd_write
};


/**************************** Hardware Functions ******************************/

/* Ideal module clock frequency for I2C */
static const Uint16 IdealModClockFrqMHz = 12;

/*
 * <summary>
 *  Initializes the GPIO for the I2C
 * </summary>
 */
static void InitI2CGpio();

/*
 * <summary>
 *  Calculates and sets the ClockDivides for the I2C Module
 * </summary>
 * <param="sysClkMhz">System Clock Frequency in Mhz</param>
 * <param="I2CClkKHz">Desired I2C Clock Frequency in KHz</param>
 */
static inline void SetClockDivides(float32 sysClkMHz, float32 I2CClkKHz);

/*
 * <summary>
 *  Initializes the I2C to run in Master Mode for a One-To-One connection
 * </summary>
 * <param="slaveAddress">Address of the slave device to write to</param>
 * <param="sysClkMhz">System Clock Frequency in Mhz</param>
 * <param="I2CClkKHz">Desired I2C Clock Frequency in KHz</param>
 */
void I2C_O2O_Master_Init(Uint16 slaveAddress, float32 sysClkMhz, float32 I2CClkKHz)
{
    // Init GPIO
    InitI2CGpio();

    EALLOW;

    // Enable Clock for I2C
    CpuSysRegs.PCLKCR9.bit.I2C_A = 1;

    // Put I2C into Reset Mode
    I2caRegs.I2CMDR.bit.IRS = 0;

    // Set Slave Address
    I2caRegs.I2CSAR.bit.SAR = slaveAddress;

    // Set Clocks
    SetClockDivides(sysClkMhz, I2CClkKHz);

    // Release from Reset Mode
    I2caRegs.I2CMDR.bit.IRS = 1;

    EDIS;
}


/*
 * <summary>
 *  Sends bytes via I2C
 * </summary>
 * <param="values">Pointer to array of bytes to send</param>
 * <param-"length">Length of array</param>
 */
void I2C_O2O_SendBytes(Uint16 * const values, Uint16 length)
{
    // Set to Master, Repeat Mode, TRX, FREE, Start
    I2caRegs.I2CMDR.all = 0x66A0;

    while(I2caRegs.I2CMDR.bit.STT){}; //wait for start condition to be cleared

    // Write values to I2C
    for (Uint16 i = 0; i < length; i++)
    {
        // Wait if Transmit is not ready
        while(!I2caRegs.I2CSTR.bit.XRDY);
        I2caRegs.I2CDXR.bit.DATA = values[i];
    }

    // Stop Bit
    I2caRegs.I2CMDR.bit.STP = 1;
}

/*
 * <summary>
 *  Calculates and sets the ClockDivides for the I2C Module
 * </summary>
 * <param="sysClkMhz">System Clock Frequency in Mhz</param>
 * <param="I2CClkKHz">Desired I2C Clock Frequency in KHz</param>
 */
static inline void SetClockDivides(float32 sysClkMHz, float32 I2CClkKHz)
{
    /* Calculate Module Clock Frequency - Must be between 7-12 MHz
     * Module Clock Frequency = sysClkMhz/(IPSC + 1)
     */
    Uint16 IPSC = (Uint16)(sysClkMHz/IdealModClockFrqMHz);

    /* Calculate Divide Downs for SCL
     * FreqMClk = sysClkMHz/((IPSC + 1)[(ICCL + d) + (ICCH + d)])
     *
     * Assume an even clock size -> ICCH == ICCL
     * ICCL = ICCH = sysclkMHz/(2000 * I2CClkKHz * (IPSC + 1)) - d
     */

    // Find value for d
    Uint16 d = 5;

    if (IPSC < 2)
    {
        d++;
        if (IPSC < 1)
        {
            d++;
        }
    }

    Uint16 ICCLH = (Uint16)(1000 * sysClkMHz/(2 * I2CClkKHz * (IPSC + 1)) - d);

    // Set values
    I2caRegs.I2CPSC.all = IPSC;
    I2caRegs.I2CCLKL = ICCLH;
    I2caRegs.I2CCLKH = ICCLH;
}


/*
 * <summary>
 *  Initializes the GPIO for the I2C
 * </summary>
 */
static void InitI2CGpio()
{

   EALLOW;
/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPDPUD.bit.GPIO104 = 0;    // Enable pull-up for GPIO32 (SDAA)
    GpioCtrlRegs.GPDPUD.bit.GPIO105 = 0;       // Enable pull-up for GPIO33 (SCLA)

/* Set qualification for selected pins to asynch only */
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPDQSEL1.bit.GPIO104 = 3;  // Asynch input GPIO32 (SDAA)
    GpioCtrlRegs.GPDQSEL1.bit.GPIO105 = 3;  // Asynch input GPIO33 (SCLA)

/* Configure SCI pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be I2C functional pins.
// Comment out other unwanted lines.

    GpioCtrlRegs.GPDGMUX1.bit.GPIO104 = 0;   // Configure GPIO104 for SDAA operation
    GpioCtrlRegs.GPDGMUX1.bit.GPIO105 = 0;   // Configure GPIO105 for SCLA operation
    GpioCtrlRegs.GPDMUX1.bit.GPIO104 = 1;
    GpioCtrlRegs.GPDMUX1.bit.GPIO105 = 1;

    //EDIS;
}
/* Name: send_command
 * Purpose: Sends an 8 bit command to the LCD accounting for all RS, E and R/~W bit logic.
 *          Delay included in this function.
 * Params: command to be written to the LCD
 */
static inline void send_command(uint16_t command)
{
    Uint16 orig = command;
    Uint16 args[4];
    //Send upper nibble with  E = 1, R/W = 0, RS = 0
    command &= 0xF0;
    command |= 0x0C;
    args[0] = command;
    //now send upper nibble with E = 0

    command &= 0xF0;
    command |= 0x08;
    args[1] = command;

   //NOW SEND LOWER NIBBLE WITH E = 1
    command = orig;
    command &= 0x0F;
    command <<=  4;
    command |= 0x0C;
    args[2] = command;

    //send lower nibble with E = 0
    command &= 0xF0;
    command |= 0x08;
    args[3] = command;

    //send out data via I2C
    I2C_O2O_SendBytes(args, sizeof(args));
}

/* Name: send_data
 * Purpose: Sends an 8 bit data to be displayed on the LCD accounting for all RS,
 *          E and R/~W bit logic.Delay included in this function.
 * Params: data to be written to the LCD
 */
static inline void send_data(char data)
{
    Uint16 orig = data;
    Uint16 args[4];

    //Send upper nibble with  E = 1, R/W = 0, RS = 1 (D0)
    data &= 0xF0;
    data |= 0x0D;
    args[0] = data;
    //now send upper nibble with E = 0

    data &= 0xF0;
    data |= 0x09;
    args[1] = data;

   //NOW SEND LOWER NIBBLE WITH E = 1
    data = orig;
    data &= 0x0F;
    data <<=  4;
    data |= 0x0D;
    args[2] = data;

    //send lower nibble with E = 0
    data &= 0xF0;
    data |= 0x09;
    args[3] = data;

    //send out data via I2C
    I2C_O2O_SendBytes(args, sizeof(args));

}
/******************************** User Functions ********************************/

/* Name: lcd_send_command
 * Purpose: Sends an 8 bit command to the LCD accounting for all RS, E and R/~W bit logic.
 *          Decodes the given command and calls the send_command function. This funciton
 *          also updates the passed lcd structs member variables
 * Params: lcd struct to update, command to be written to the LCD
 */
void lcd_send_command(lcd_t *lcd, lcd_command_t command)
{
    switch(command)
    {
    case LCD_NEWLINE:
        send_command(LCD_NEWLINE);
        lcd->cursor_pos = 16;
        break;

    case LCD_RESET_CURSOR:
        send_command(LCD_RESET_CURSOR);
        lcd->cursor_pos = 0;
        break;

    case LCD_CLEAR_SCREEN:
        send_command(LCD_CLEAR_SCREEN);
        lcd->cursor_pos = 0;
        for(uint16_t i = 0; i < LCD_ROW_SIZE; i++)
        {
            for(uint16_t j = 0; j < LCD_COL_SIZE; j++)
            {
                lcd->display[i][j] = 0;
            }
        }
        break;

    case LCD_INIT33:
        send_command(LCD_INIT33);
        break;

    case LCD_INIT32:
        send_command(LCD_INIT32);
        break;

    case LCD_4B_2L:
        send_command(LCD_4B_2L);
        break;

    case LCD_ALL_ON:
        send_command(LCD_ALL_ON);
        break;

    default:
        return;
    }

    LCD_delay

}

/* Name: lcd_init
 * Purpose: Initializes the LCD to 4 Bit 2 Line mode, clears screen and resets cursor.
 * Params: lcd struct to initialize
 */
void lcd_init(lcd_t *lcd)
{
    I2C_O2O_Master_Init(LCD_I2C_ADDR, 200, LCD_I2C_SPEED_MHZ);
    lcd_send_command(lcd, LCD_CLEAR_SCREEN);
    lcd_send_command(lcd, LCD_INIT33);
    lcd_send_command(lcd, LCD_INIT32);
    lcd_send_command(lcd, LCD_4B_2L);
    lcd_send_command(lcd, LCD_ALL_ON);
    lcd_send_command(lcd, LCD_CLEAR_SCREEN);
    lcd_send_command(lcd, LCD_RESET_CURSOR);
}

/* Name: lcd_write
 * Purpose: writes passed message string to the LCD. The message is wrapped to the second line
 *          if the message is too long for the first line. If the message is longer than can be displayed
 *          on the entire LCD, the screen is cleared and cursor is reset, and writing continues.
 * Params: lcd struct to update. message string to be written to the LCD
 */
void lcd_write(lcd_t *lcd, char *message)
{
    uint16_t i = (lcd->cursor_pos >= 15) ? 1 : 0;
    uint16_t j = lcd->cursor_pos % (LCD_COL_SIZE);
    char *curr = message;
    while(*curr != 0)
    {

        //check for /n escape sequence, which is 0x0a in ascii
        if (*curr == (char)0x0a)
        {
            //send out new line command
            lcd->send_command(lcd, LCD_NEWLINE);
            i = 1;
            j = 0;
            curr++; //skip new line sequence
        }


        if(i == (LCD_ROW_SIZE -1) && j == (LCD_COL_SIZE))
        {
            //clear screen and refresh cursor
            lcd->send_command(lcd, LCD_CLEAR_SCREEN);
            lcd->send_command(lcd, LCD_RESET_CURSOR);
            j = 0;
            i = 0;
        }
        else if(j == LCD_COL_SIZE )
        {
            //send out new line command
            lcd->send_command(lcd, LCD_NEWLINE);
            i = 1;
            j = 0;
        }

        send_data(*curr);
        lcd->display[i][j] = *curr;
        lcd->cursor_pos++;
        curr++;
        j++;
        LCD_delay
    }
}


//this function takes in a hex value and returns the character representation in decimal
char * to_string(float value, uint16_t int_digits, uint16_t float_digits)
{
    //char arr[int_digits+float_digits+1+1]; //number of characters is the digits displayed +1 for the decimal point, +1 for  null
    char *arr = (char*)calloc((int_digits+float_digits+2), sizeof(char));
    char *c_ptr = arr;
    float tens = 1;
    uint16_t int_digits_copy = int_digits;
    while(int_digits_copy != 1)
    {
        tens = tens*10;
        int_digits_copy--;
    }

    for(uint16_t i = 0; i < int_digits; i++)
    {
        uint16_t digit = (uint16_t)((value)/tens); //get digit starting with most significant digit
        value = (float)(value - ((float)digit)*tens);
        tens = tens/10;
        *c_ptr = (char)(digit + 0x30);
        c_ptr++;
    }

    *(c_ptr++) = '.'; //add decimal point

    uint16_t tenths = 1;
    float float_digits_copy = float_digits;

    while(float_digits_copy != 0)
    {
        tenths = tenths*10;
        float_digits_copy--;
    }

    value = value * tenths; //shift decimal point over
    tenths = tenths/10.0;
    uint16_t digit;
    for(uint16_t i = 0; i < float_digits; i++)
    {
        digit = ((value)/((float)tenths)); //get digit starting with most significant digit
        value = (float)(value - ((float)digit)*tenths);
        tenths = ((float)tenths)/10.0;
        *c_ptr = (char)(digit + 0x30);
        c_ptr++;
    }
    *c_ptr = 0;
    c_ptr = arr;
    return c_ptr;
}
