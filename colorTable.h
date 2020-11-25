#ifndef COLOR_TABLE_H
#define COLOR_TABLE_H

#include <avr/pgmspace.h>
#include <stdint.h>

#pragma pack(1)
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

const extern struct Color fire[256] PROGMEM;
const extern struct Color blueSee[256] PROGMEM;
const extern struct Color jungle[256] PROGMEM;
const extern struct Color night[256] PROGMEM;
struct Color readColorFromFlash(__flash struct Color const * colorValue);

#pragma pack()

#endif
