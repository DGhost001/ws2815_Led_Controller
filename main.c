#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "colorTable.h"
#include "keyboard.h"
#include "leds.h"

#define ELEMENT_COUNT(X) (sizeof(X) / sizeof (X[0]))

static uint8_t leds[660];
static uint8_t noisePoints[2][22];
static uint8_t ctrlPoints[ELEMENT_COUNT(noisePoints[0])];
static __flash struct Color const * currentTable;

static uint16_t xorShift( )
{
    static uint16_t state = 0x12fe;

    state ^= state << 7;
    state ^= state >> 9;
    state ^= state << 8;

    return state;
}

static void initNoisePoints()
{
    for(unsigned i = 0; i < ELEMENT_COUNT(noisePoints[0]); ++i) {
        noisePoints[0][i] = xorShift() & 0xff;
        noisePoints[1][i] = xorShift() & 0xff;
    }
}

static void moveNoisePoints()
{
    for(unsigned i = 0; i < ELEMENT_COUNT(noisePoints[0]); ++i) {
        noisePoints[0][i] = noisePoints[1][i];
        noisePoints[1][i] = xorShift() & 0xff;
    }

}

static uint8_t interpolate(uint8_t a, uint8_t b, uint8_t offset)
{
    float const t = offset/255.0;
    float const w =  t*t*(3-2*t);
    return round(a*(1-w) + b*w);
}

static void computeCtrlPoints( uint8_t const offset )
{
    for(unsigned i = 0; i < ELEMENT_COUNT(ctrlPoints); ++i) {
        ctrlPoints[i] = interpolate(noisePoints[0][i], noisePoints[1][i], offset);
    }
}

static void computeLeds()
{
    for(unsigned i = 0; i< ELEMENT_COUNT(leds); ++i) {
        const uint8_t a =  i / 32;
        const uint8_t b =  a + 1;
        const uint8_t o = (i % 32) * 8;
        leds[i] = interpolate(ctrlPoints[a], ctrlPoints[b], o);
    }
}

static void sendLeds()
{
    struct Color color;
    
    cli();
    
    for(unsigned i = 0; i< ELEMENT_COUNT(leds); ++i) {
        color = readColorFromFlash(&currentTable[leds[i]]);
        sendColor(&color);
    }
    
    sei();
}

static void handleKeyBoard( void )
{
    if(isKeyPressed(KEY_1)/* && isKeyToggled(KEY_1) */) {
        currentTable = &fire[0];
    }

    if(isKeyPressed(KEY_2) /* && isKeyToggled(KEY_2) */) {
        currentTable = &blueSee[0];
    }

    if(isKeyPressed(KEY_3) /* && isKeyToggled(KEY_3) */) {
        currentTable = &jungle[0];
    }

    if(isKeyPressed(KEY_5) /* && isKeyToggled(KEY_5) */) {
        currentTable = &night[0];
    }
}

ISR (TIMER0_OVF_vect)
{
    readKeyBoard();
    handleKeyBoard();
}

int main( void )
{
    uint8_t offset = 0;
    currentTable = &fire[0];

    initLeds();
    initNoisePoints();
    initKeyBoard();
    
    TCCR0 = (1<<CS01) | (1<<CS00); //Set Prescaler to 64 --> 488 Hz c.a. 2ms
    TIMSK |= (1<<TOIE0);
    sei();
    

    while(true) {
        computeCtrlPoints(offset);
        computeLeds();
        sendLeds();
        ++offset;

        if(offset == 0) {
            moveNoisePoints();
        }
        //__builtin_avr_delay_cycles(500000);
    }
    return 0;
}
