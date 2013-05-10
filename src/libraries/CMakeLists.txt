# Build the contributed Arduino libraries

set(ardlibsfiles "")
foreach(dir Wire Wire/utility)
	file(GLOB tmpfiles ${libspath}/${dir}/*.c ${libspath}/${dir}/*.cpp)
	list(APPEND ardlibsfiles ${tmpfiles}) 
	include_directories( ${libspath}/${dir} )
endforeach(dir)
add_library (arduinolibs ${ardlibsfiles})

set(st7565libfiles "")

message(st7565dir=${ST7565_DIR})
file(GLOB st7565libsfiles ${ST7565_DIR}/*.c ${ST7565_DIR}/*.cpp)
include_directories( ${ST7565_DIR} )
add_library (st7565 ${st7565libsfiles})

set(timelibfiles "")
foreach(dir Time)
	file(GLOB tmpfiles ./${dir}/*.c ./${dir}/*.cpp)
	list(APPEND timelibsfiles ${tmpfiles}) 
	include_directories( ./${dir} )
endforeach(dir)
add_library (time ${timelibsfiles})

add_subdirectory ( Dogm )

#message(STATUS "libspath=${libspath}")
#message(STATUS "libsfiles=${libsfiles}")


