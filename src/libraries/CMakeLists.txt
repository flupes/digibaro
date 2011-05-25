# Build the contributed Arduino libraries

set(libsfiles "")

foreach(dir Wire Wire/utility)
	file(GLOB tmpfiles ${libspath}/${dir}/*.c ${libspath}/${dir}/*.cpp)
#	message(STATUS "path_to_lib=${libspath}/${dir}")
#	message(STATUS "tmpfiles=${tmpfiles}")
	list(APPEND libsfiles ${tmpfiles}) 
	include_directories( ${libspath}/${dir} )
endforeach(dir)

foreach(dir ST7565)
	file(GLOB tmpfiles ./${dir}/*.c ./${dir}/*.cpp)
	list(APPEND libsfiles ${tmpfiles}) 
	include_directories( ./${dir} )
endforeach(dir)

foreach(dir Time)
	file(GLOB tmpfiles ./${dir}/*.c ./${dir}/*.cpp)
	list(APPEND libsfiles ${tmpfiles}) 
	include_directories( ./${dir} )
endforeach(dir)

message(STATUS "libspath=${libspath}")
message(STATUS "libsfiles=${libsfiles}")

add_library (arduinolibs ${libsfiles})

#file(GLOB coreheaders *.h)

