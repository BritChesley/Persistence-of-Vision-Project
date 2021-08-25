#include <F28x_Project.h>
#include <driverlib.h>

#ifndef _FTDI_SPI
#define _FTDI_SPI
#define DSP_CS_GPIO 72

typedef enum state{
    visual = 0xFF05,
    audio = 0x776F
}state_t;




void spic_init_gpio();
void spic_init_slave();
void init_usb_spi();

void rx_image_data();

#endif
