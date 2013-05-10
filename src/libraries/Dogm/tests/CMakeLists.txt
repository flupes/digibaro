# Where to find the includes
include_directories( ${dogm_dir} )

add_library (dogmcpp ${dogm_dir}/Dogm.cpp )

add_program( GraphicTest dogmcpp dogm corearduino )
add_program( DogmFonts dogmcpp dogm corearduino )
add_program( FontSpeed dogmcpp dogm corearduino )
