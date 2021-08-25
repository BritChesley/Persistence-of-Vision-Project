
#ifndef LCD_H_
#define LCD_H_

/********************************* Defines **********************************/
#define LCD_DISPLAY_SIZE    32      //Total number of characters that can be written
#define LCD_COL_SIZE        16      //number of characters in each column
#define LCD_ROW_SIZE        2       //number of rows for our LCD
#define LCD_I2C_ADDR        0x3F    //I2C address
#define LCD_I2C_SPEED_MHZ   100     //I2C speed for LCD
/****************************** Typedef Enums *******************************/

/*
 * This typedef defines all commands that can be given to the LCD
 */
typedef enum LCD_COMMANDS{
    LCD_NEWLINE         = 0xC0,
    LCD_RESET_CURSOR    = 0x80,
    LCD_CLEAR_SCREEN    = 0x01,
    LCD_INIT33          = 0x33,
    LCD_INIT32          = 0x32,
    LCD_4B_2L           = 0x28,
    LCD_ALL_ON          = 0x0F
}lcd_command_t;

/******************************* Structs ************************************/

/*
 * This struct defines the main LCD struct that grants the user a hardware
 * abstracted interface.
 */
typedef struct lcd_struct{
    char display[LCD_ROW_SIZE][LCD_COL_SIZE]; //message displayed on LCD
    uint16_t    cursor_pos;                   //cursor position
    void (*init) (struct lcd_struct *lcd);    //lcd initialization function pointer
    void (*write)(struct lcd_struct *lcd, char *message);                   //lcd write function pointer
    void (*send_command)(struct lcd_struct *lcd, lcd_command_t command);    //lcd send command function pointer
}lcd_t;


/************************ Hardware Specific functions ***********************/
#define LCD_delay for(volatile long i = 0; i < 150000; i++); //modified by Brit Chesley

/*
 * <summary>
 *  Initializes the I2C to run in Master Mode for a One-To-One connection
 * </summary>
 * <param="slaveAddress">Address of the slave device to write to</param>
 * <param="sysClkMhz">System Clock Frequency in Mhz</param>
 * <param="I2CClkKHz">Desired I2C Clock Frequency in KHz</param>
 */
void I2C_O2O_Master_Init(Uint16 slaveAddress, float32 sysClkMhz, float32 I2CClkKHz);

/*
 * <summary>
 *  Sends bytes via I2C
 * </summary>
 * <param="values">Pointer to array of bytes to send</param>
 * <param-"length">Length of array</param>
 */
void I2C_O2O_SendBytes(Uint16 * const values, Uint16 length);


/****************************** User Functions ******************************/

/* Name: lcd_init
 * Purpose: Initializes the LCD to 4 Bit 2 Line mode, clears screen and resets cursor.
 * Params: lcd struct to initialize
 */
void lcd_init(lcd_t *lcd);

/* Name: lcd_send_command
 * Purpose: Sends an 8 bit command to the LCD accounting for all RS, E and R/~W bit logic.
 *          Decodes the given command and calls the send_command function. This funciton
 *          also updates the passed lcd structs member variables
 * Params: lcd struct to update, command to be written to the LCD
 */
void lcd_send_command(lcd_t *lcd, lcd_command_t command);

/* Name: lcd_write
 * Purpose: writes passed message string to the LCD. The message is wrapped to the second line
 *          if the message is too long for the first line. If the message is longer than can be displayed
 *          on the entire LCD, the screen is cleared and cursor is reset, and writing continues.
 * Params: lcd struct to update. message string to be written to the LCD
 */
void lcd_write(lcd_t *lcd, char *message);

char * to_string(float value, uint16_t int_digits, uint16_t float_digits);
#endif /* LCD_H_ */
