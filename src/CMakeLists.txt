# Top level makefile for some Arduino related projects

# probably would work with earlier versions, but we currently run 2.8
cmake_minimum_required(VERSION 2.8)

# This is project "digibaro" !
project(digibaro)

# If we want to see what is going on, turn to ON
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Use the exact Arduino IDE flags
#set(ARDUINO_IDE_MODE TRUE)

# Path to third-party libraries
set(EXTERNAL_GIT_DIR /Volumes/ljfdev/git)
set(EXTERNAL_PKG_DIR /Volumes/ljfdev/pkgs)

# Where to find the ST7565 by Adafruit
set(ST7565_DIR ${EXTERNAL_GIT_DIR}/ST7565-LCD/ST7565)
message(hereitis=${ST7565_DIR})

include( cmake/Arduino.cmake )

# Here are the Arduino core libraries to build
add_subdirectory( cores )
add_subdirectory( libraries )

# Add project specific subdirs
add_subdirectory( experiments )
add_subdirectory( EepromUtils )
add_subdirectory( AnalogFiveButtons )
add_subdirectory( DigitalBaro )
