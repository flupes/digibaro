# Build the contributed Arduino libraries

set(ardlibsfiles "")
foreach(dir Wire Wire/utility)
	file(GLOB tmpfiles ${libspath}/${dir}/*.c ${libspath}/${dir}/*.cpp)
	list(APPEND ardlibsfiles ${tmpfiles}) 
	include_directories( ${libspath}/${dir} )
endforeach(dir)
add_library (arduinolibs ${ardlibsfiles})

set(st7565libfiles "")
foreach(dir ST7565)
	file(GLOB tmpfiles ./${dir}/*.c ./${dir}/*.cpp)
	list(APPEND st7565libsfiles ${tmpfiles}) 
	include_directories( ./${dir} )
endforeach(dir)
add_library (st7565 ${st7565libsfiles})

set(timelibfiles "")
foreach(dir Time)
	file(GLOB tmpfiles ./${dir}/*.c ./${dir}/*.cpp)
	list(APPEND timelibsfiles ${tmpfiles}) 
	include_directories( ./${dir} )
endforeach(dir)
add_library (time ${timelibsfiles})

#message(STATUS "libspath=${libspath}")
#message(STATUS "libsfiles=${libsfiles}")


