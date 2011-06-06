# Configure to use the Adafruit ST7565P graphic LCD
add_definitions( -DADA_ST7565P )
    	
file( GLOB dogmfiles ./utility/*.c )
include_directories( ./utilitty )

add_library (dogm ${dogmfiles})

add_subdirectory ( tests )
