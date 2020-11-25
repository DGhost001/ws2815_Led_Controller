#include "leds.h"

#include <avr/io.h>



void initLeds( void )
{
    DDRB    = (1<<DDB0); //Initialze PINB0 as Output
}

    // 0 --> HI Clocks 2  | LOW --> 12 (> 5) Clocks
    // 1 --> HI 12 (>5) Clocks | LOW -->  2 Clocks

    // brcs 1 - false; 2 - true
    // LSL  1

#define START_SEND(REG) "lsl " #REG "\n"
#define CONT_SEND(REG) \
     "brcc 1f\n"\
     "sbi 0x18,0\n"\
     "lsl " #REG "\n" \
     "nop\n"\
     "nop\n"\
     "nop\n"\
     "cbi 0x18,0\n"\
     "rjmp 2f\n"\
     "1:sbi 0x18,0\n"\
     "cbi 0x18,0\n"\
     "lsl " #REG "\n"\
     "nop\n"\
     "nop\n"\
     "nop\n"\
     "nop\n"\
     "2:\n"

#define BYTE_SEND(REG1, REG2) \
             CONT_SEND(REG1)\
             CONT_SEND(REG1)\
             CONT_SEND(REG1)\
             CONT_SEND(REG1)\
             CONT_SEND(REG1)\
             CONT_SEND(REG1)\
             CONT_SEND(REG1)\
             CONT_SEND(REG2)\


void sendColor(struct Color const * const color)
{
    uint8_t b1 = color->g;
    uint8_t b2 = color->r;
    uint8_t b3 = color->b;

    __asm__ volatile ( START_SEND(%0)
             BYTE_SEND(%0,%1)
             BYTE_SEND(%1,%2)
             BYTE_SEND(%2,%2) :
             "+r" (b1),
             "+r" (b2),
             "+r" (b3)::"memory","cc" );
}
