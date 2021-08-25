#ifndef _DUAL_SRAM_H_
#define _DUAL_SRAM_H_

/************************************ Includes ************************************/
#include <stdint.h>
/************************************ Defines *************************************/
#define BYTE_WRITE_INSTR    0x02
#define SEQ_WR_INSTR        0x02
#define SEQ_RD_INSTR        0x03

#define SRAM1_BOUND_WORD    0x1FFFF //128K
#define SRAM1_BOUND_BYTE    0x3FFFF
#define SRAM2_START_WORD    0x20000
#define SRAM2_END_WORD      0x3FFFF
#define SRAM_SIZE_WORD      262144 //256 K
#define SRAM_SIZE_BYTE      (SRAM_SIZE_WORD << 1)

/*********************************** Structs **************************************/
/*Struct name: dual_sram_struct
 *             typedef'd as sram_t
 *This struct holds all relevant functions and buffers for communicating
 *with the SRAM board
 */
typedef struct dual_sram_struct{
    uint16_t *write_buffer;
    uint16_t *read_buffer;
    void (*init) (struct dual_sram_struct *sram);    //sram initialization function
    void (*read_block)(struct dual_sram_struct * sram, uint32_t start_word_addr, uint16_t *buffer, uint32_t num_words); //read block function
    void (*write_block)(struct dual_sram_struct *sram, uint32_t start_word_addr, uint16_t* data, uint32_t num_words); //write block function
    uint16_t (*read_word)(struct dual_sram_struct *sram, uint32_t word_addr); //read word function
    void (*write_word)(struct dual_sram_struct *sram, uint32_t word_addr, uint16_t data); //write word function

}sram_t;

/***************************** Function Declarations ******************************/
/* Name: sram_board_init
 * Purpose: This function calls the two previous initialization functions
 * Params: sram struct
 * Returns: Void
 */
void sram_board_init(sram_t *sram);

/* Name: sram_read_word_HS
 * Purpose: This function is used to read a single word from a generic SRAM. All addressing logic
 *          is taken care off. This function sets chip selects
 * Params: sram struct, 32 bit word aligned address
 * Returns: read 16 bit data
 */
uint16_t sram_read_word_HS(sram_t *sram, uint32_t word_addr);

/* Name: sram_write_word_HS
 * Purpose: This function is used to write a single word to a generic SRAM. All addressing logic
 *          is taken care off. This function sets chip selects appropriately
 * Params: sram struct, 32 bit word aligned address, and Data to write to the SRAM
 * Returns: Void
 */
void sram_write_word_HS(sram_t *sram, uint32_t word_addr, uint16_t data);

/* Name: sram_write_block_HS
 * Purpose: This function is used to write an array of words to a generic SRAM. All addressing logic
 *          is taken care off. This function sets chip selects
 * Params: sram struct, 32 bit word aligned starting address, array of data to write, and number of words to write
 * Returns: Void
 */
void sram_write_block_HS(sram_t * sram, uint32_t start_word_addr, uint16_t *data, uint32_t num_words);

/* Name: sram_read_block_HS
 * Purpose: This function is used to read from the SRAM and place all read data into the
 *          the passed buffer. All addressing logic is taken care off. This function sets chip selects
 * Params: sram struct, 32 bit word aligned starting address, buffer to place read data,
 *         and number of words to read
 * Returns: Void
 */
void sram_read_block_HS(sram_t * sram, uint32_t start_word_addr, uint16_t *buffer, uint32_t num_words);

#endif
