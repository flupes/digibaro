# Build the contributed Arduino libraries

set(EXTERNAL_GIT_DIR /Volumes/MacNetDisk/ljfdev/git)
set(EXTERNAL_PKG_DIR /Volumes/MacNetDisk/ljfdev/pkgs)

set(ardlibsfiles "")
foreach(dir Wire Wire/utility)
	file(GLOB tmpfiles ${libspath}/${dir}/*.c ${libspath}/${dir}/*.cpp)
	list(APPEND ardlibsfiles ${tmpfiles}) 
	include_directories( ${libspath}/${dir} )
endforeach(dir)
add_library (arduinolibs ${ardlibsfiles})

set(st7565libfiles "")

foreach(dir ${EXTERNAL_GIT_DIR}/ST7565-LCD/ST7565)
	file(GLOB tmpfiles ${dir}/*.c ${dir}/*.cpp)
	list(APPEND st7565libsfiles ${tmpfiles}) 
	include_directories( ${dir} )
endforeach(dir)
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


