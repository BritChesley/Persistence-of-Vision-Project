#pragma once
#include <windows.h>
#include "ftd2xx.h"
#include <stdio.h>
#include <stdint.h>

typedef enum state {
	visual = 0x05FF,
	audio = 0x6F77
}state_t;






#define BUFFER_SIZE (1024) //1K buffer size
#define USB_TRANSFER_SIZE (5*1024) //5K for now, must be multiple of 64 bytes
#define DEFAULT_PIN_DIRS (0b00001011)



/** Commands given to serial engine **/
#define SET_DLOW_BITS 0x80
#define CLOCK_BYTE_OUT 0x11
#define SET_CLOCK_DIV 0x86





/************** Function Declarations ***************/
FT_STATUS send_data_spi(FT_HANDLE *ftHandle, uint8_t *data, uint16_t buffer_len);
