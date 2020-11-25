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

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ledStripeController
        ${SOURCE}
)
