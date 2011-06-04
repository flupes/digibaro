# Build the contributed Arduino libraries

set(libsfiles "")

foreach(dir Wire Wire/utility)
	file(GLOB tmpfiles ${libspath}/${dir}/*.c ${libspath}/${dir}/*.cpp)
#	message(STATUS "path_to_lib=${libspath}/${dir}")
#	message(STATUS "tmpfiles=${tmpfiles}")
	list(APPEND libsfiles ${tmpfiles}) 
	include_directories( ${libspath}/${dir} )
endforeach(dir)

foreach(dir ST7565 Time)
	file(GLOB tmpfiles ./${dir}/*.c ./${dir}/*.cpp)
	list(APPEND libsfiles ${tmpfiles}) 
	include_directories( ./${dir} )
endforeach(dir)

foreach(dir Dogm Dogm/utility)
	file(GLOB tmpfiles ./${dir}/*.c ./${dir}/*.cpp)
	list(APPEND libsfiles ${tmpfiles}) 
	include_directories( ./${dir} )
endforeach(dir)
add_definitions( -DADA_ST7565P )

message(STATUS "libspath=${libspath}")
message(STATUS "libsfiles=${libsfiles}")

add_library (arduinolibs ${libsfiles})

#file(GLOB coreheaders *.h)

