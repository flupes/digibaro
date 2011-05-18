#!/bin/sh

callcmd=$0
workdir=`pwd`
scriptname=${0##*/}

os=`uname -s`

# We assume that the avr binaries are already in the path
# this variable is to find the right config file under Win32
avr_home="C:\Arduino22\hardware\tools\avr"

if [ ${os} = "Linux" ] ; then
    default_port=/dev/ttyACM0
    avr_conf=/etc/avrdude.conf
    programmer=arduino
else
    # Default COM port
    default_port=COM7
    avr_conf="${avr_home}\etc\avrdude.conf"
    programmer=stk500v1
fi

if [ $# -lt 1 ] ; then
    echo "Usage: ${scriptname} file_to_upload.hex [com_port]"
    echo
    exit 1
else    

    if [ ${os} = "Cygwin" ] ; then
	hexfile=`cygpath -w $1`
    else
	hexfile=$1
    fi

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

avrdude -C ${avr_conf} -p atmega328p -V -c ${programmer} -P ${port} -U flash:w:${hexfile}
