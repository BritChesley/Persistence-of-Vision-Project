#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "dual_sram.h"
#include <stdbool.h>

/******************** Structs *********************/
typedef struct circ_buff{
    uint32_t read_count;
    uint32_t write_count;
    uint32_t capacity; //total size
    uint32_t start_addr; //internal
    void (*init)(struct circ_buff *buff, sram_t *sram, uint32_t capacity);
    void (*write)(struct circ_buff *buff, sram_t *sram, uint32_t index, int16_t data);
    void (*read)(struct circ_buff *buff, sram_t *sram, uint32_t index, int16_t *data);
    void (*reset)(struct circ_buff *buff);
}circ_buffer_t;

/*************** Function Declarations *************/
//initializes buffer
void buffer_init(circ_buffer_t *buff, sram_t *sram, uint32_t capacity);

//writes to buffer at specified index in SRAM
void buffer_write(circ_buffer_t *buff, sram_t *sram, uint32_t index, int16_t data);

//reads from buffer at specified index in sram, places value in *data
void buffer_read(circ_buffer_t *buff, sram_t *sram, uint32_t index, int16_t *data);

//resets certain internal member variables of the sram
void buffer_reset(circ_buffer_t *buff);
#endif
