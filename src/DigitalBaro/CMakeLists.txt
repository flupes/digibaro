# Define the local library
add_library(dbaro
    Bmp085.cpp
    Ds3231.cpp
    WeatherLcdGraph.cpp
)

set(LIBS eepromUtils analog5b arduinolibs corearduino m)

# Where to find the includes
include_directories( ${libspath}/Wire )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/ST7565 )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/Time )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../AnalogFiveButtons )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../EepromUtils )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

# Create the executable
add_program( DigitalBaro dbaro st7565 time ${LIBS} )
add_program( testFonts arduinolibs st7565 corearduino m)

add_subdirectory ( tests )
