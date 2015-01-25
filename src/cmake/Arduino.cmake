# ============================================================
# Paths for the toolchain
# ============================================================

# Where to find the Arduino IDE (hardcoded for now...)
if ( ARDUINO_ROOT )
  message(STATUS "ARDUINO_ROOT was defined externally to: ${ARDUINO_ROOT}")
elseif ( CMAKE_HOST_WIN32 )
  set(ARDUINO_ROOT "C:/Arduino22")
#elseif ( CYGWIN )
#  set(ARDUINO_ROOT "/cygdrive/c/Arduino22")
# will not work since host is Unix in case of Cygwin
elseif ( CMAKE_HOST_APPLE )
  set(ARDUINO_ROOT "/Users/Shared/arduino/Arduino.app/Contents/Resources/Java")
elseif ( CMAKE_HOST_UNIX )
  set(ARDUINO_ROOT "/usr/share/arduino")
else ()
  message(FATAL_ERROR "Plaform not supported... Define ARDUINO_ROOT")
endif()

# Location "cores" files under the Arduino IDE
set(CORES_ARDUINO_DIR "hardware/arduino/cores/arduino")

# Default relative location of the Arduino libraries
set(ARDUINO_LIBS_DIR "libraries")

# Path the the AVR toolchain
set(AVR_TOOLCHAIN_DIR "hardware/tools/avr/bin")
set(ARDUINO_AVR_BIN ${ARDUINO_ROOT}/${AVR_TOOLCHAIN_DIR})

# Define the full paths
set(corespath ${ARDUINO_ROOT}/${CORES_ARDUINO_DIR})
#set(corespath ${CMAKE_SOURCE_DIR}/cores)
set(libspath ${ARDUINO_ROOT}/${ARDUINO_LIBS_DIR})

# general includes of cores files
include_directories( ${corespath} )

# Define the Arduino IDE version
set(ARDUINO_FLAG "-DARDUINO=106")

# ============================================================
# Compilation and linking definitions
# ============================================================

if(ARDUINO_IDE_MODE)
    set(WARNING_FLAGS "-w")
    set(C_FLAGS "")
    set(TUNING_FLAGS "")
message("This is IDE mode")
else()
    set(WARNING_FLAGS "-Wall -Wextra")
    set(C_FLAGS "-Wstrict-prototypes")
    set(TUNING_FLAGS "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums")
message("This regular mode")
endif()

set(DEBUG_FLAG "-g")
set(OPTI_FLAG "-Os")
# what to do with -MMD from the IDE?
set(CXX_FLAGS "-fno-exceptions -DUSB_VID=null -DUSB_PID=null")
set(AVR_FLAGS "-ffunction-sections -fdata-sections")
set(LINKOPTS "-Wl,--gc-sections")

# Set the target options: looks at the file Arduino/hardware/arduino/boards.txt
if( NOT DEFINED BOARD )
  message(FATAL_ERROR
    "Board not specified: use -D BOARD=YourBoard on cmake invocation!")
endif()

find_file( BOARD_CONFIG_FILE ${BOARD}.cmake 
  PATH ${CMAKE_SOURCE_DIR}/cmake
  DOC "Arduino board configuration"
  )

if ( BOARD_CONFIG_FILE )
  include( ${BOARD_CONFIG_FILE} )
else ()
  message(FATAL_ERROR
    "Configuration for board [${BOARD}] not found in ${CMAKE_SOURCE_DIR}/cmake")
endif ()

set(MCU_FLAG "-mmcu=${BOARD_MCU}")
set(CPU_FLAG "-DF_CPU=${BOARD_CPU}")

# Add the board specific includes
include_directories( ${ARDUINO_ROOT}/hardware/arduino/variants/${BOARD_VARIANT} )

# Define the C compiler options
set(CMAKE_C_FLAGS "${DEBUG_FLAG} ${OPTI_FLAG} ${WARNING_FLAGS} ${C_FLAGS} ${AVR_FLAGS} ${MCU_FLAG} ${CPU_FLAG} ${ARDUINO_FLAG}")

# Define the C++ compiler options
set(CMAKE_CXX_FLAGS "${DEBUG_FLAG} ${OPTI_FLAG} ${WARNING_FLAGS} ${CXX_FLAGS} ${AVR_FLAGS} ${MCU_FLAG} ${CPU_FLAG} ${ARDUINO_FLAG}")

# Define the linker options
set(CMAKE_EXE_LINKER_FLAGS "${OPTI_FLAG} ${MCU_FLAG} ${LINKOPTS}")


# ============================================================
# Definitions for avr-objcopy
# ============================================================
set(ARDUINO_ELF_SUFFIX elf)
set(ARDUINO_HEX_SUFFIX hex)

set(OBJCPY_BIN avr-objcopy)
set(OBJCPY_OPTS -O ihex -R .eeprom)
#set(OBJCPY_OPTS "-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0")

set(AVRSIZE_BIN avr-size)
set(AVRSIZE_OPTS --format=avr --mcu=${BOARD_MCU})


# ============================================================
#
# add_program: macro defining how to build a binary to upload on the board
# 
# arg: PGMNAME = name of the program to build, link and convert for eeprom
# arg: ARGN (all extra arguments) = libraries to include in the link
# ============================================================
macro( add_program PGMNAME )
  set( ARDUINO_PGMELF ${PGMNAME}.${ARDUINO_ELF_SUFFIX} )
  set( ARDUINO_PGMHEX ${PGMNAME}.${ARDUINO_HEX_SUFFIX} )
  add_executable( ${ARDUINO_PGMELF} ${PGMNAME}.cpp )
  target_link_libraries( ${ARDUINO_PGMELF} ${ARGN} )
  set_target_properties( ${ARDUINO_PGMELF} PROPERTIES LINKER_LANGUAGE "C" )
  add_custom_command( TARGET ${ARDUINO_PGMELF} 
    POST_BUILD
    COMMAND ${OBJCPY_BIN}
    ARGS ${OBJCPY_OPTS} ${ARDUINO_PGMELF} ${ARDUINO_PGMHEX}
    COMMAND ${AVRSIZE_BIN}
    ARGS ${AVRSIZE_OPTS} ${ARDUINO_PGMELF}
    COMMENT "Creating eeprom binary for ${PGMNAME} ..."
#    VERBATIM
    )
  install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${ARDUINO_PGMHEX} DESTINATION ${CMAKE_BINARY_DIR}/hex )
endmacro( add_program PGMNAME )
