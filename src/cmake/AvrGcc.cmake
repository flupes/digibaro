# ------------------------------------------------------------
#
# Configure CMake to cross compile for Arduino (avr-gcc)
#
# Lorenzo Flueckiger - May 2011
#
# ------------------------------------------------------------

# ============================================================
# We cross compile with avr-gcc
# ============================================================
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_RANLIB "echo \"do not run ranlib!\"")
set(BUILD_SHARED_LIBS FALSE)


