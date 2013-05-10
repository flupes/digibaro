# Build the core Arduino library

file(GLOB coresfiles ${corespath}/*.c ${corespath}/*.cpp)

message(STATUS "corespath=${corespath}")
message(STATUS "coresfiles=${coresfiles}")

add_library (corearduino ${coresfiles})

file(GLOB coreheaders *.h)

#install(FILES coreheaders DESTINATION include)
#install(TARGETS corearduino ARCHIVE DESTINATION lib)

