#include <codec.h>
#include <stdint.h>
#include "AIC23.h"
#include <F28x_Project.h>


/************************* Initalizing codec struct ******************************/
volatile codec_board_t codec = {
                        .leds = {LED_OFF, LED_OFF, LED_OFF, LED_OFF, LED_OFF, LED_OFF, LED_OFF, LED_OFF},
                        .data_ready = false,
                        .io_init = &codec_io_init,
                        .sound_init = &codec_sound_init,
                        .led_write = &codec_led_write,
                        .dip_read = &codec_dip_read,
                        .pb_read = &codec_pb_read,
                        .rx_audio = &codec_read_audio,
                        .tx_audio = &codec_transmit_audio,
                        .data_ready = &codec_data_ready,
                        .pb_int_enable = &codec_io_pb_int_enable
                      };

/******************************* Functions ***************************************/

/*Function: codec_board_io_init
 *Description: Sets up all GPIO for the LEDs, DIP switches, and PB Switches
 *Params none
 */
void codec_io_init()
{
    EALLOW;
    GpioCtrlRegs.GPAMUX1.all &= ~0xF0FFFFFF; //this sets pins (Pins 13 and 14 shoudl be left unchanged)
    GpioCtrlRegs.GPAMUX2.all &= ~0x00000003; //sets GPIO16 to be GPIO

    GpioCtrlRegs.GPAGMUX1.all &= ~0xF0FFFFFF; //set to GPIO mode
    GpioCtrlRegs.GPAGMUX2.all &= ~0x00000003;

    GpioCtrlRegs.GPAPUD.all &= ~0x0001CF00; //enable pull up resistors on GPIO 8:11, GPIO14:16

    GpioCtrlRegs.GPADIR.all &= ~0x0001CF00; //set GPIO11:8, GPIO16:14 as inputs

    GpioDataRegs.GPASET.all |= 0x00FF; //set LEDs to be off
    GpioCtrlRegs.GPADIR.all |= 0x00FF; //set GPIO7:0 as outputs
}

void codec_sound_init(codec_board_t *codec, uint32_t SRMODE)
{
    InitBigBangedCodecSPI(); //why are we using bit banged code?
    InitMcBSPb();
    InitAIC23(SRMODE); //48 KHz
    codec->left_data_rx = 0;
    codec->right_data_rx = 0;
    codec->left_data_tx = 0;
    codec->right_data_tx = 0;
}

/*Function: codec_led_write
 * Description: Writes the value of led_value to the codecs LEDs
 * Params: @led_value is the ACTIVE HIGH value to write to the LEDs
 */
void codec_led_write(codec_board_t *codec, uint16_t led_value)
{
    EALLOW;
    led_value = led_value & 0xFF; //make sure to only write to the bottom byte
    GpioDataRegs.GPACLEAR.all = led_value;
    GpioDataRegs.GPASET.all = ~led_value;
    led_value = ~led_value;
    //update codec struct
    for(int i = 0; i < 8; i++)
    {
         codec->leds[i] = (led_state_t)((led_value & (1<<i))>>i);
    }
}

/*Function: codec_pb_read
 *Description: reads a value from the PB switches Codec struct is updated with the values
 *Params: @codec is the struct that is modified with the PB switch values.
 *Params:  Also returns a uint16_t with the hex value value masked from DAT register
 */
uint16_t codec_pb_read(codec_board_t *codec)
{
    EALLOW;
    uint32_t hex_value = 0;
    //GPA16:14
     hex_value = ((GpioDataRegs.GPADAT.all) & (0x01C000)) >> 14;

     for(int i = 0 ; i <= 2; i++)
     {
         codec->pb_sw[i] = (pb_sw_state_t)((hex_value & (1<<i))>>i);
     }
     return hex_value;
}

/*Function: codec_dip_read
 *Description: reads a value from the DIP switches Codec struct is updated with the values
 *Params: @codec is the struct that is modified with the DIP switch values.
 *Params:  Also returns a uint16_t with the hex value masked from DAT register
 */
uint16_t codec_dip_read(codec_board_t *codec)
{
    EALLOW;
    uint16_t hex_value = 0;
    //GPA11:8
    hex_value = ((GpioDataRegs.GPADAT.all) & (0xF00)) >> 8; //mask bits we care about

    for(int i = 0; i <= 3; i++)
    {
       codec->dip_sw[i] = (dip_sw_state_t)((hex_value & (1<<i))>>i);
    }
    return hex_value;
}

/*Function: codec_read_audio
 *Description: reads both left and right channels, Codec struct is updated with the values.
 *             data ready variable also marked true
 *Params: @codec is the struct that has the modified rx variables updated.
 */
void codec_read_audio(codec_board_t *codec)
{
    codec->left_data_rx = McbspbRegs.DRR2.all;
    codec->right_data_rx = McbspbRegs.DRR1.all;
    codec->_data_ready = true; //set bool
}

void codec_io_pb_int_enable(codec_board_t *codec, uint16_t push_buttons, void (*pb1_isr)(void), void (*pb2_isr)(void), void (*pb3_isr)(void))
{
//    Interrupt_register(INT_XINT3,  pb3_isr);
//
//
//    GPIO_setInterruptPin(14,GPIO_INT_XINT3); //pb3
//    GPIO_setInterruptPin(15,GPIO_INT_XINT4); //pb2
//    GPIO_setInterruptPin(16,GPIO_INT_XINT5); //pb3
//    GPIO_setInterruptType(GPIO_INT_XINT3, GPIO_INT_TYPE_FALLING_EDGE);
//    GPIO_setInterruptType(GPIO_INT_XINT4, GPIO_INT_TYPE_FALLING_EDGE);
//    GPIO_setInterruptType(GPIO_INT_XINT5, GPIO_INT_TYPE_FALLING_EDGE);
//
//
//
//    GPIO_enableInterrupt(GPIO_INT_XINT3);         // Enable XINT3
//    GPIO_enableInterrupt(GPIO_INT_XINT4);         // Enable XINT4
//    GPIO_enableInterrupt(GPIO_INT_XINT5);         // Enable XINT5
//
//    Interrupt_enable(INT_XINT3); //all in PIE group 12
//    Interrupt_enable(INT_XINT4);
//    Interrupt_enable(INT_XINT5);
    uint16_t shift = 1;
    for(int16_t i = 1; i < 5; i+=0)
    {
        if((push_buttons & shift) == shift)
        {
            if(i == 1)
            {
                GPIO_setInterruptPin(14,GPIO_INT_XINT3); //pb3
                GPIO_setInterruptType(GPIO_INT_XINT3, GPIO_INT_TYPE_FALLING_EDGE);
                Interrupt_register(INT_XINT3, pb3_isr);
                GPIO_enableInterrupt(GPIO_INT_XINT3);
                Interrupt_enable(INT_XINT3);
            }
            else if(i == 2)
            {
                GPIO_setInterruptPin(15,GPIO_INT_XINT4); //pb2
                GPIO_setInterruptType(GPIO_INT_XINT4, GPIO_INT_TYPE_FALLING_EDGE);
                Interrupt_register(INT_XINT4, pb2_isr);
                GPIO_enableInterrupt(GPIO_INT_XINT4);
                Interrupt_enable(INT_XINT4);
            }
            else if(i == 3)
            {
                GPIO_setInterruptPin(16,GPIO_INT_XINT5); //pb1
                GPIO_setInterruptType(GPIO_INT_XINT5, GPIO_INT_TYPE_FALLING_EDGE);
                Interrupt_register(INT_XINT5, pb1_isr);
                GPIO_enableInterrupt(GPIO_INT_XINT5);
                Interrupt_enable(INT_XINT5);
            }
        }
        i++;
        shift = shift<<1;
    }
}

/*Function: codec_transmit_audio
 *Description: outputs both the left and right TX channel data from struct
 *Params: @codec is the struct that has Tx variables to output
 */
void codec_transmit_audio(codec_board_t *codec)
{
    McbspbRegs.DXR2.all = codec->left_data_tx;
    McbspbRegs.DXR1.all = codec->right_data_tx;
}

/*Function: codec_data_ready
 *Description: returns true if new data received, false otherwise. uses internal _data_ready
 *              variable and updates this variable appropriately
 *Params: @codec is the struct that has internal _data_ready flag
 */
bool codec_data_ready(codec_board_t *codec)
{
    bool ret = codec->_data_ready;
    if(ret)
    {
        codec->_data_ready = false;
    }
    return ret;
}
