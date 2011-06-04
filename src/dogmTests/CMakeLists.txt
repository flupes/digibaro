# Where to find the includes
include_directories( ${libspath}/Wire )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libraries/Dogm )

add_definitions( -DADA_ST7565P )

add_program( GraphicTest arduinolibs corearduino )
add_program( Fonts arduinolibs corearduino )

