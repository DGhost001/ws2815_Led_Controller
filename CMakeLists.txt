cmake_minimum_required(VERSION 2.6)

include(${CMAKE_TOOLCHAIN_FILE})

project(ledStripeController)
include_directories(${PROJECT_SOURCE_DIR})

SET(SOURCE
    main.c
    colorTable.c
    crc16.c
    keyboard.c
    leds.c
    colorTable.h
    crc16.h
    keyboard.h
    leds.h
)

SET(EXECUTABLE ledStripeController)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${EXECUTABLE}
        ${SOURCE}
)

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND avr-size --format=berkeley ${EXECUTABLE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

add_custom_target(flash avr-objcopy -j .text -j .data -O ihex ${EXECUTABLE} ${EXECUTABLE}.ihex
                  COMMAND avrdude -c stk500 -P /dev/ttyUSB1 -p atmega16 -U flash:w:${EXECUTABLE}.ihex:i
                  DEPENDS ${EXECUTABLE}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  BYPRODUCTS ${EXECUTABLE}.ihex
                  )

add_custom_target(disasm avr-objdump -DS -z ${EXECUTABLE} 2>&1 > ${EXECUTABLE}.S
                  DEPENDS ${EXECUTABLE}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  BYPRODUCTS ${EXECUTABLE}.S
                  )
