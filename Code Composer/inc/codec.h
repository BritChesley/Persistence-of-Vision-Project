#ifndef _CODEC_H
#define _CODEC_H


/***************************** codec_io.h ********************************/

/*************************************************************************
This file contains all necessary declarations and definitions
to communicate with the codec boards' LEDs, DIP switches, and PB switches

**************************************************************************/

/*************************** Dependencies ********************************/
#include <F28x_Project.h>
#include "driverlib.h"
#include "AIC23.h"
/******************************** Enums **********************************/

/*This enum describes the state of the low-true LEDs
 * LEDs are on
 */
typedef enum led_state{
    LED_ON  = 0,
    LED_OFF = 1
}led_state_t;

/*
 * This enum describes the DIP switch states
 */
typedef enum dip_sw_state{
    SW_ON  = 0,
    SW_OFF = 1
}dip_sw_state_t;

/*
 * This enum describes the PB switch states
 */
typedef enum pb_sw_state{
    PB_PUSHED   = 0,
    PB_RELEASED = 1
}pb_sw_state_t;


enum LEDS{
    LED0 = 0x01,
    LED1 = 0x02,
    LED2 = 0x04,
    LED3 = 0x08,
    LED4 = 0x10,
    LED5 = 0x20,
    LED6 = 0x40,
    LED7 = 0x80
};

enum PB{
    PB3 = 0x01,
    PB2 = 0x02,
    PB1 = 0x04
};

enum DIP{
    DIP1 = 0x01,
    DIP2 = 0x02,
    DIP3 = 0x04,
    DIP4 = 0x08
};
/******************************* Structs **********************************/

typedef struct codec_board_s{

    volatile led_state_t     leds[8];    //8 on board LEDs, GPIO7:0  => GPA7:0
    volatile dip_sw_state_t  dip_sw[4];  //4 dip switches, GPIO11:8  => GPA11:8
    volatile pb_sw_state_t   pb_sw[3];   //3 PB switches, GPIO16:14  => GPA16:14
    int16_t left_data_rx;
    int16_t right_data_rx;
    int16_t left_data_tx;
    int16_t right_data_tx;
    bool _data_ready;
    void (*io_init)(void);
    void (*sound_init)(struct codec_board_s *codec, uint32_t SRMODE); //initializes codec for DSP mode
    void (*rx_audio)(struct codec_board_s *codec);
    void (*tx_audio)(struct codec_board_s *codec);
    bool (*data_ready)(struct codec_board_s *codec);
    void (*led_write)(struct codec_board_s *codec, uint16_t led_value);
    uint16_t (*pb_read)(struct codec_board_s *codec);
    uint16_t (*dip_read)(struct codec_board_s *codec);
    void (*pb_int_enable)(struct codec_board_s *codec, uint16_t push_buttons, void (*pb1_isr)(void), void (*pb2_isr)(void),void (*pb3_isr)(void));
}codec_board_t;

/************************ Function Declarations ***************************/
/*Function: codec_board_io_init
 *Description: Sets up all GPIO for the LEDs, DIP switches, and PB Switches
 *Params none
 */
void codec_io_init();

void codec_io_pb_int_enable(codec_board_t *codec, uint16_t push_buttons, void (*pb1_isr)(void), void (*pb2_isr)(void), void (*pb3_isr)(void));

/*Function: codec_sound_init
 *Description: Sets up all Codec for DSP mode at 48KHz, sets up interruot
 *Params none
 */
void codec_sound_init(codec_board_t *codec, uint32_t SRMODE);

/*Function: codec_led_write
 * Description: Writes the value of led_value to the codecs LEDs
 * Params: @led_value is the ACTIVE HIGH value to write to the LEDs
 */
void codec_led_write(codec_board_t *codec, uint16_t led_value);

/*Function: codec_pb_read
 *Description: reads a value from the PB switches Codec struct is updated with the values
 *Params: @codec is the struct that is modified with the PB switch values.
 *Params:  Also returns a uint16_t with the hex value value masked from DAT register
 */
uint16_t codec_pb_read(codec_board_t *codec);

/*Function: codec_dip_read
 *Description: reads a value from the DIP switches Codec struct is updated with the values
 *Params: @codec is the struct that is modified with the DIP switch values.
 *Params:  Also returns a uint16_t with the hex value masked from DAT register
 */
uint16_t codec_dip_read(codec_board_t *codec);

/*Function: codec_read_audio
 *Description: reads both left and right channels, Codec struct is updated with the values
 *Params: @codec is the struct that has the modified rx variables updated
 */
void codec_read_audio(codec_board_t *codec);

/*Function: codec_transmit_audio
 *Description: outputs both the left and right TX channel data from struct
 *Params: @codec is the struct that has Tx variables to output
 */
void codec_transmit_audio(codec_board_t *codec);

/*Function: codec_data_ready
 *Description: returns true if new data received, false otherwise. uses internal _data_ready
 *              variable and updates this variable appropriately
 *Params: @codec is the struct that has internal _data_ready flag
 */
bool codec_data_ready(codec_board_t *codec);
#endif
