# Build the test programs

set(LIBS dbaro eepromUtils arduinolibs corearduino)

# Create the executables
add_program(testDeciPascal ${LIBS})
add_program(testWeatherLcdGraph ${LIBS} u8glib )
add_program(testRtcInterrupts ${LIBS})
