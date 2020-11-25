#ifndef LEDS_H
#define LEDS_H

#include "colorTable.h"

/**
  * This inits the PORTS for the LEDS
  */
void initLeds( void );

/**
  * This sends a Color out to the LEDS
  */
void sendColor(struct Color const * const color);

#endif