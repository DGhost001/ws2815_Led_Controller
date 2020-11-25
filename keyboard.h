#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

enum Keys {
    KEY_1 = 12,
    KEY_2 = 13,
    KEY_3 = 14,
    KEY_4 = 15,
    KEY_5 = 8,
    KEY_6 =  9,
    KEY_7 = 10,
    KEY_8 = 11,
    KEY_9 = 4,
    KEY_10 = 5,
    KEY_11 = 6,
    KEY_12 = 7,
    KEY_13 = 0,
    KEY_14 = 1,
    KEY_15 = 2,
    KEY_16 = 3
};

/**
 * @brief initKeyBoard will initalize the keyboard
 */
void initKeyBoard( void );

/**
 * @brief readKeyBoard updated the pressed keys in the current row and advances the readout to the next row
 */
void readKeyBoard( void );

/**
 * @brief isKeyPressed check if the key is pressed
 * @retval True - The key is pressed
 * @retval False - The key is released
 */

bool isKeyPressed( enum Keys);

/**
 * @brief isKeyToggled returns true if the key was toggled lately
 * @retval True - The key was toggled
 * @retval False - The key was not toggled
 */
bool isKeyToggled( enum Keys);

#endif
