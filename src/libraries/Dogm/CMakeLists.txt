# Configure to use the Adafruit ST7565P graphic LCD
add_definitions( -DADA_ST7565P_HW -DDOG_DOUBLE_MEMORY )
    	
set(dogm_dir ${EXTERNAL_PKG_DIR}/Dogm)
file( GLOB dogmfiles ${dogm_dir}/utility/*.c )
include_directories( ${dogm_dir}/utility )

add_library (dogm ${dogmfiles})

add_subdirectory ( tests )
