/*
 * led_blade.h
 *
 *  Created on: Nov 15, 2019
 *      Author: Bobby Burwell
 */

#ifndef INC_LED_BLADE_H_
#define INC_LED_BLADE_H_

#include <F28x_Project.h>
#include <F2837xD_device.h>
#include <stdint.h>
#include <driverlib.h>

// Using 8 Bit Mode
#define RBITS 3
#define GBITS 3
#define BBITS 2

// Using 8 Bit Mode
#define RBITS 3
#define GBITS 3
#define BBITS 2

#define RSTART (RBITS-1)
#define GSTOP (RBITS)
#define GSTART (7-BBITS)
#define BSTOP (8-BBITS)

#define BZEROPAD (7-BBITS)
#define GZEROPAD (7-GBITS)
#define RZEROPAD (7-RBITS)
#define RSTART (RBITS-1)
#define GSTOP (RBITS)
#define GSTART (7-BBITS)
#define BSTOP (8-BBITS)


#define BZEROPAD (7-BBITS)
#define GZEROPAD (7-GBITS)
#define RZEROPAD (7-RBITS)



// Defines
#define delays 116
#define delayTime 2
#define LED_PORT GPIO_PORT_C
/*
#define LED_CLK 0
#define LED_D0 3
*/
#define LED_CLK 83
#define LED_D0 85
#define LED_D1 84

#define CLK_BM 0x80000
#define D1_BM  0x100000
#define D0_BM  0x200000


// Transmission States
#define NUM_DIVISIONS 200
#define NUM_LEDS 25
#define LED_CHANNELS 2
#define TRANSACTION_SIZE (NUM_LEDS)
#define GLOBAL_BRGHTNSS1 0x2
#define GLOBAL_BRGHTNSS0 0x2

#define START_FRAME 0x00000000
#define END_FRAME 0xFFFFFFFF

#define CLK_LOW 0
#define CLK_HIGH 1

//uint32_t transmission[TRANSMISSION_SIZE];

typedef struct{
    uint32_t frame0[NUM_LEDS];
    uint32_t frame1[NUM_LEDS];
}transaction_t;

typedef struct{
    transaction_t flash[NUM_DIVISIONS];
}picture_t;

typedef struct{
    uint8_t brightness;
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint32_t value;
}led_frame_t;

typedef struct{
    uint16_t frame[NUM_LEDS];
}byte_trans_t;

void startFrame() {
    for (int ii = 31; ii >= 0; ii--){
        GPIO_writePin(LED_CLK, 0);
        GPIO_writePin(LED_D0, 0);
        GPIO_writePin(LED_D1, 0);
        GPIO_writePin(LED_CLK, 1);
    }
    GPIO_writePin(LED_CLK, 0);
}

void endFrame() {
    for (int ii = 31; ii >= 0; ii--){
        GPIO_writePin(LED_CLK, 0);
        GPIO_writePin(LED_D0, 1);
        GPIO_writePin(LED_D1, 1);
        GPIO_writePin(LED_CLK, 1);
    }
    GPIO_writePin(LED_CLK, 0);
}

void sendFrame(uint32_t frame0, uint32_t frame1) {
    uint8_t startSeq0 = (0xD0 | GLOBAL_BRGHTNSS0);
    uint8_t startSeq1 = (0xD0 | GLOBAL_BRGHTNSS1);
    for (int ii = 7; ii >= 0; ii--){
        GPIO_writePin(LED_CLK, 0);
        GPIO_writePin(LED_D0, ((startSeq0>>ii)&(0x01)));
        GPIO_writePin(LED_D1, ((startSeq1>>ii)&(0x01)));
        GPIO_writePin(LED_CLK, 1);
    }
    for (int ii = 23; ii >= 0; ii--){
        GPIO_writePin(LED_CLK, 0);
        GPIO_writePin(LED_D0, ((frame0>>ii)&(0x01)));
        GPIO_writePin(LED_D1, ((frame1>>ii)&(0x01)));
        GPIO_writePin(LED_CLK, 1);
    }
    GPIO_writePin(LED_CLK, 0);
}

void clearFrame() {
    uint8_t startSeq0 = (0xD0 | GLOBAL_BRGHTNSS0);
    uint8_t startSeq1 = (0xD0 | GLOBAL_BRGHTNSS1);
    for (int ii = 7; ii >= 0; ii--){
        GPIO_writePin(LED_CLK, 0);
        GPIO_writePin(LED_D0, ((startSeq0>>ii)&(0x01)));
        GPIO_writePin(LED_D1, ((startSeq1>>ii)&(0x01)));
        GPIO_writePin(LED_CLK, 1);
    }
    for (int ii = 23; ii >= 0; ii--){
        GPIO_writePin(LED_CLK, 0);
        GPIO_writePin(LED_D0, 0);
        GPIO_writePin(LED_D1, 0);
        GPIO_writePin(LED_CLK, 1);
    }
    GPIO_writePin(LED_CLK, 0);
}

void clearTrans() {
    startFrame();
    for (int jj = 0; jj < TRANSACTION_SIZE; jj++){
        clearFrame();
    }
    endFrame();
}


#ifdef _FLASH
__attribute__((ramfunc))
#endif
void sendMagnitude(uint16_t mag0, uint32_t col0, uint16_t mag1, uint32_t col1) {
    startFrame();
    for (int jj = 0; jj < 25; jj++){
        sendFrame((uint32_t)(((bool) mag0)*col0),(uint32_t)(((bool) mag1)*col1));
        if(mag0){
            mag0--;
        }
        if(mag1){
            mag1--;
        }
    }
    endFrame();
}

void sendTransmission(transaction_t transaction) {
    startFrame();
    for (int jj = 0; jj < TRANSACTION_SIZE; jj++){
        sendFrame(transaction.frame0[jj],transaction.frame1[jj]);
    }
    endFrame();
}

void sendFlippedTransmission(transaction_t transaction) {
    startFrame();
    for (int jj = 0; jj < TRANSACTION_SIZE; jj++){
        sendFrame(transaction.frame1[jj],transaction.frame0[jj]);
    }
    endFrame();
}
#ifdef _FLASH
__attribute__((ramfunc))
#endif
void sendWord(uint16_t col) {
    //Start Sequence
    uint16_t startSeq = ((0xD0 | GLOBAL_BRGHTNSS0)<<8)|(0xD0 | GLOBAL_BRGHTNSS1);
    for (int ii = 15; ii >= 8; ii--){
        GPIO_writePin(LED_CLK, 0);
        GPIO_writePin(LED_D0, ((startSeq>>ii)&(0x01)));
        GPIO_writePin(LED_D1, ((startSeq>>(ii-8))&(0x01)));
        GPIO_writePin(LED_CLK, 1);
    }
    // Send Upper Two Blue Bits (Bits 7-6)
    for (int jj = 0; jj < 4; jj++){
        for (int ii = 15; ii >= 14; ii--){
            GPIO_writePin(LED_CLK, 0);
            GPIO_writePin(LED_D0, ((col>>ii)&(0x01)));
            GPIO_writePin(LED_D1, ((col>>(ii-8))&(0x01)));
            GPIO_writePin(LED_CLK, 1);
        }
    }
    // Send Three Green Bits (Bits 5-3)
    for (int jj = 0; jj < 2; jj++){
        for (int ii = 13; ii >= 11; ii--){
            GPIO_writePin(LED_CLK, 0);
            GPIO_writePin(LED_D0, ((col>>ii)&(0x01)));
            GPIO_writePin(LED_D1, ((col>>(ii-8))&(0x01)));
            GPIO_writePin(LED_CLK, 1);
        }
    }
    for (int ii = 13; ii >= 12; ii--){
        GPIO_writePin(LED_CLK, 0);
        GPIO_writePin(LED_D0, ((col>>ii)&(0x01)));
        GPIO_writePin(LED_D1, ((col>>(ii-8))&(0x01)));
        GPIO_writePin(LED_CLK, 1);
    }
    // Send Three Red Bits (Bits 2-0)
    for (int jj = 0; jj < 2; jj++){
        for (int ii = 10; ii >= 8; ii--){
            GPIO_writePin(LED_CLK, 0);
            GPIO_writePin(LED_D0, ((col>>ii)&(0x01)));
            GPIO_writePin(LED_D1, ((col>>(ii-8))&(0x01)));
            GPIO_writePin(LED_CLK, 1);
        }
    }
    for (int ii = 10; ii >= 9; ii--){
        GPIO_writePin(LED_CLK, 0);
        GPIO_writePin(LED_D0, ((col>>ii)&(0x01)));
        GPIO_writePin(LED_D1, ((col>>(ii-8))&(0x01)));
        GPIO_writePin(LED_CLK, 1);
    }
    GPIO_writePin(LED_CLK, 0);
}
#ifdef _FLASH
__attribute__((ramfunc))
#endif
void sendFlippedWord(uint16_t col) {
    //Start Sequence
    uint16_t startSeq = ((0xD0 | GLOBAL_BRGHTNSS1)<<8)|(0xD0 | GLOBAL_BRGHTNSS0);
    for (int ii = 15; ii >= 8; ii--){
        GPIO_writePin(LED_CLK, 0);
        GPIO_writePin(LED_D1, ((startSeq>>ii)&(0x01)));
        GPIO_writePin(LED_D0, ((startSeq>>(ii-8))&(0x01)));
        GPIO_writePin(LED_CLK, 1);
    }
    // Send Upper Two Blue Bits (Bits 7-6)
    for (int jj = 0; jj < 4; jj++){
        for (int ii = 15; ii >= 14; ii--){
            GPIO_writePin(LED_CLK, 0);
            GPIO_writePin(LED_D1, ((col>>ii)&(0x01)));
            GPIO_writePin(LED_D0, ((col>>(ii-8))&(0x01)));
            GPIO_writePin(LED_CLK, 1);
        }
    }
    // Send Three Green Bits (Bits 5-3)
    for (int jj = 0; jj < 2; jj++){
        for (int ii = 13; ii >= 11; ii--){
            GPIO_writePin(LED_CLK, 0);
            GPIO_writePin(LED_D1, ((col>>ii)&(0x01)));
            GPIO_writePin(LED_D0, ((col>>(ii-8))&(0x01)));
            GPIO_writePin(LED_CLK, 1);
        }
    }
    for (int ii = 13; ii >= 12; ii--){
        GPIO_writePin(LED_CLK, 0);
        GPIO_writePin(LED_D1, ((col>>ii)&(0x01)));
        GPIO_writePin(LED_D0, ((col>>(ii-8))&(0x01)));
        GPIO_writePin(LED_CLK, 1);
    }
    // Send Three Red Bits (Bits 2-0)
    for (int jj = 0; jj < 2; jj++){
        for (int ii = 10; ii >= 8; ii--){
            GPIO_writePin(LED_CLK, 0);
            GPIO_writePin(LED_D1, ((col>>ii)&(0x01)));
            GPIO_writePin(LED_D0, ((col>>(ii-8))&(0x01)));
            GPIO_writePin(LED_CLK, 1);
        }
    }
    for (int ii = 10; ii >= 9; ii--){
        GPIO_writePin(LED_CLK, 0);
        GPIO_writePin(LED_D1, ((col>>ii)&(0x01)));
        GPIO_writePin(LED_D0, ((col>>(ii-8))&(0x01)));
        GPIO_writePin(LED_CLK, 1);
    }
    GPIO_writePin(LED_CLK, 0);
}
#ifdef _FLASH
__attribute__((ramfunc))
#endif
void sendWordTransmission(byte_trans_t transaction) {
    startFrame();
    for (int jj = 0; jj < TRANSACTION_SIZE; jj++){
        sendWord(transaction.frame[jj]);
    }
    endFrame();
}
#ifdef _FLASH
__attribute__((ramfunc))
#endif
void sendWordFlippedTransmission(byte_trans_t transaction) {
    startFrame();
    for (int jj = 0; jj < TRANSACTION_SIZE; jj++){
        sendFlippedWord(transaction.frame[jj]);
    }
    endFrame();
}

#endif /* INC_LED_BLADE_H_ */
