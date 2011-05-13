#!/bin/sh

callcmd=$0
workdir=`pwd`
scriptname=${0##*/}

# Default COM port
default_port=COM7

# We assume that the avr binaries are already in the path
# this variable is to find the right config file
avr_root="C:\Arduino22\hardware\tools\avr"

if [ $# -lt 1 ] ; then
    echo "Usage: ${scriptname} file_to_upload.hex [com_port]"
    echo
    exit 1
else    
    hexfile=$1
    if [ $# -eq 2 ] ; then
	port=$2
    else
	port=${default_port}
    fi
fi

# Check if we are at the top of the build directory
if [ ! -e ./CMakeCache.txt ] ; then
    echo "Please run ${scriptname} from the root of the build directory."
    exit 1
fi

build_core_flags=./cores/CMakeFiles/corearduino.dir/flags.make

mmcu=`grep C_FLAGS ${build_core_flags} | sed -n 's/^.*-mmcu=\(.*\) -DF_CPU.*$/\1/p'`
# | tr '[:lower:]' '[:upper:]' `

avrdude -C "${avr_root}\etc\avrdude.conf" -p ${mmcu} -V -c stk500v1 -P ${port} -U flash:w:.\\hex\\${hexfile}.hex
