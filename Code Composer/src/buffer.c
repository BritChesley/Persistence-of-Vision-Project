/************************ Includes ***********************/
#include "buffer.h"

/********************* Struct Definition *****************/
volatile circ_buffer_t buffer = {
                                 .init = &buffer_init,
                                 .read = &buffer_read,
                                 .write = &buffer_write,
                                 .reset = &buffer_reset
};

/********************** Static Variables *****************/
static uint32_t sram_global_address = 0;

/******************** Function Definitions ***************/
//sets all values in buffer to zero
static inline void buffer_clear(circ_buffer_t *buff, sram_t *sram)
{
    //clear buffer
    for(uint32_t i = 0; i < SRAM_SIZE_WORD; i++)
    {
        sram->write_word(sram, i, 0x00); //all data to zero
    }
}
//initializes buffer
void buffer_init(circ_buffer_t *buff, sram_t *sram, uint32_t capacity)
{
    buffer_clear(buff, sram);
    buff->capacity = capacity;
    buff->read_count = 0;
    buff->write_count = 0;
    buff->start_addr = sram_global_address;
    sram_global_address += capacity;
}

//writes to buffer at specified index
void buffer_write(circ_buffer_t *buff, sram_t *sram, uint32_t index, int16_t data)
{    uint32_t addr = index & (buff->capacity - 1);
    sram->write_word(sram, addr + buff->start_addr, data);
}

//read from buffer at specified index, puts data in *data
void buffer_read(circ_buffer_t *buff, sram_t *sram, uint32_t index, int16_t *data)
{
    uint32_t addr = index & (buff->capacity - 1);
    *data = sram->read_word(sram, addr+buff->start_addr); //update data container
}

//resets read_count and write_count, I dont really use this
void buffer_reset(circ_buffer_t *buff)
{
    buff->read_count = 0;
    buff->write_count = 0;
}
