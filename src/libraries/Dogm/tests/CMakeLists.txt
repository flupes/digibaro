# Where to find the includes
include_directories( ./ ../ )

add_library (dogmcpp Dogm.cpp )

add_program( GraphicTest dogmcpp dogm corearduino )
add_program( DogmFonts dogmcpp dogm corearduino )
add_program( FontSpeed dogmcpp dogm corearduino )

