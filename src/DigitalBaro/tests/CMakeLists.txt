# Build the test programs

set(LIBS dbaro arduinolibs corearduino)

# Create the executables
add_program(testDeciPascal ${LIBS})

