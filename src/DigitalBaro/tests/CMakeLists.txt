# Build the test programs

set(LIBS dbaro eepromUtils arduinolibs corearduino)

# Create the executables
add_program(testDeciPascal ${LIBS})
add_program(testWeatherLcdGraph ${LIBS})
