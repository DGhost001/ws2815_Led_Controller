#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "colorTable.h"
#include "keyboard.h"
#include "leds.h"
#include "settings.h"

#define ELEMENT_COUNT(X) (sizeof(X) / sizeof (X[0]))

enum {
    saveTimeOut = 2 * 400 //Press save for about 2 sec
};

static uint8_t leds[660];
static uint8_t noisePoints[2][22];
static uint8_t ctrlPoints[ELEMENT_COUNT(noisePoints[0])];
static volatile uint16_t save;
static struct Settings * settings;


static uint16_t xorShift( void )
{
    static uint16_t state = 0x12fe;

    state ^= state << 7;
    state ^= state >> 9;
    state ^= state << 8;

    return state;
}

static void initNoisePoints( void )
{
    for(unsigned i = 0; i < ELEMENT_COUNT(noisePoints[0]); ++i) {
        noisePoints[0][i] = xorShift() & 0xff;
        noisePoints[1][i] = xorShift() & 0xff;
    }
}

static void moveNoisePoints( void )
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

static void computeLeds( void )
{
    for(unsigned i = 0; i< ELEMENT_COUNT(leds); ++i) {
        const uint8_t a =  i / 32;
        const uint8_t b =  a + 1;
        const uint8_t o = (i % 32) * 8;
        leds[i] = interpolate(ctrlPoints[a], ctrlPoints[b], o);
    }
}

static __flash struct Color const * getColorTable( void )
{
    switch (settings->mode) {
    case 0: return &fire[0];
    case 1: return &blueSee[0];
    case 2: return &jungle[0];
    case 3: return &night[0];
    default: return &fire[0];
    }
}


static void sendLeds( void )
{
    struct Color color;
    __flash struct Color const * const currentTable = getColorTable();

    {
        cli();

        for(unsigned i = 0; i< ELEMENT_COUNT(leds); ++i) {
            color = readColorFromFlash(&currentTable[leds[i]]);
            sendColor(&color);
        }

        sei();
    }
}


static void setAllLedsToColor(struct Color const * const color)
{
    cli();

    for(unsigned i = 0; i< ELEMENT_COUNT(leds); ++i) {
         sendColor(color);
    }

    sei();

    _delay_us(500); //There needs to be at least a 500us gap for the LEDs to commit the Value
}

static inline void inc(uint8_t * value) {
    if(*value < 255) ++(*value);
}

static inline void dec(uint8_t * value) {
    if(*value > 0) --(*value);
}


static void handleKeyBoard( void )
{
    if(save > 0) { //User Input is disabled during save process.
        if(isKeyPressed(KEY_1)/* && isKeyToggled(KEY_1) */) {
            settings->mode = 0;
        }

        if(isKeyPressed(KEY_2) /* && isKeyToggled(KEY_2) */) {
            settings->mode = 1;
        }

        if(isKeyPressed(KEY_3) /* && isKeyToggled(KEY_3) */) {
            settings->mode = 2;
        }

        if(isKeyPressed(KEY_5) /* && isKeyToggled(KEY_5) */) {
            settings->mode = 3;
        }

        if(isKeyPressed(KEY_13)) {
            settings->mode = 9;
        }

        if(isKeyPressed(KEY_14)) {
            settings->mode = 10;
        }

        if(isKeyPressed(KEY_15)) {
            settings->mode = 11;
        }

        if(isKeyPressed(KEY_16)) {
            settings->mode = 12;
        }

        if(isKeyPressed(KEY_12)) {
            switch (settings->mode) {
            case 9: inc(&settings->color.r); break;
            case 10: inc(&settings->color.g); break;
            case 11: inc(&settings->color.b); break;
            case 12: inc(&settings->color.r); inc(&settings->color.g); inc(&settings->color.b); break;
            default: break;
            }
        }

        if(isKeyPressed(KEY_8)) {
            switch (settings->mode) {
            case 9: dec(&settings->color.r); break;
            case 10: dec(&settings->color.g); break;
            case 11: dec(&settings->color.b); break;
            case 12: dec(&settings->color.r); dec(&settings->color.g); dec(&settings->color.b); break;
            default: break;
            }
        }

        if(isKeyPressed(KEY_4)) {
            if(save > 0) --save;
        } else {
            save = saveTimeOut;
        }
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

    save     = saveTimeOut;
    settings = loadSettings();

    if(!settings) {
        settings = newSettings();
    }

    initLeds();
    initNoisePoints();
    initKeyBoard();
    
    TCCR0 = (1<<CS01) | (1<<CS00); //Set Prescaler to 64 --> 488 Hz c.a. 2ms
    TIMSK |= (1<<TOIE0);
    sei();
    

    while(true) {

        if(settings->mode < 9) {
            computeCtrlPoints(offset);
            computeLeds();
            sendLeds();
            ++offset;

            if(offset == 0) {
                moveNoisePoints();
            }
        } else {
             setAllLedsToColor(&settings->color);
        }

        if(0 == save) {
            struct Color color = {.r = 0, .g = 0, .b = 0};
            setAllLedsToColor(&color); //Blank all LEDs to Signal Save in Progress
            saveSettings();
            save = saveTimeOut;
        }

        //__builtin_avr_delay_cycles(500000);
    }
    return 0;
}
