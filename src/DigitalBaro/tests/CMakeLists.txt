# Build the test programs

set(LIBS dbaro eepromUtils arduinolibs corearduino)

# Create the executables
add_program(testDeciPascal ${LIBS})
add_program(testWeatherLcdGraph st7565 ${LIBS})
add_program(testRtcInterrupts ${LIBS})
