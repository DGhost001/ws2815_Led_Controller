#include "keyboard.h"
#include <avr/io.h>

static uint8_t currentShift = 0x00;
static uint16_t currentKeysPressed;
static uint16_t previousKeysPressed;

void initKeyBoard( void )
{
    DDRA  = 0xf0; // Enable the upper 4 bits as output
    //DDRD  = 0xff;
    PORTA = (0x10 << currentShift); //Output a 1 to the first column
    currentKeysPressed = 0;
}

void readKeyBoard( void )
{
    uint16_t const mask = 0xf;
    uint16_t const key  = PINA & mask;
    uint16_t const shift = currentShift * 4;

    previousKeysPressed = currentKeysPressed;
    currentKeysPressed = (currentKeysPressed & (~(mask << shift))) | (key << shift);  
    currentShift = (currentShift + 1) & 3;
    
    //PORTD = currentKeysPressed & 0xff;

    PORTA = 0x10 << currentShift; //Output to the next Col
}


bool isKeyPressed( enum Keys key)
{
    return (currentKeysPressed & (1<<key)) != 0;
}

bool isKeyToggled( enum Keys key)
{
    return ((previousKeysPressed ^ currentKeysPressed) & (1<<key)) != 0;
}
