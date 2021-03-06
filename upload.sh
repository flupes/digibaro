#!/bin/sh

callcmd=$0
workdir=`pwd`
scriptname=${0##*/}

# We assume that the avr binaries are already in the path
# this variable is to find the right config file under Win32 only
win32_avr_home="C:\Arduino22\hardware\tools\avr"

# Arduino location under Mac OS-x
darwin_avr_home="/Users/Shared/arduino/Arduino.app/Contents/Resources/Java/hardware/tools/avr"

baudrate=57600

uos=`uname`
case $uos in
    Linux*)
	default_port=/dev/ttyACM0
	avr_conf=/etc/avrdude.conf
	programmer=arduino
	;;
    Darwin*)
	default_port=/dev/tty.usbmodem1d11131 
	avr_conf=${darwin_avr_home}/etc/avrdude.conf
	programmer=arduino
	;;
    CYGWIN*|MINGW32*)
	#Default COM port
	default_port=COM7
	avr_conf="${win32_avr_home}\etc\avrdude.conf"
	programmer=stk500v1
	;;
    *)
	echo "OS [${uos}] is not supported yet..."
esac	


if [ $# -lt 1 ] ; then
    echo "Usage: ${scriptname} file_to_upload.hex [com_port]"
    echo
    exit 1
else    

    if [ ${uos} = "Cygwin" ] ; then
	hexfile=`cygpath -w $1`
    else
	hexfile=$1
    fi

    if [ $# -eq 2 ] ; then
	port=$2
    else
	port=${default_port}
    fi

    options=""
    if [ $# -eq 3 ] ; then
	options=$3
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

avrdude -C ${avr_conf} ${options} -p atmega328p -c ${programmer} -P ${port} -b ${baudrate} -D -U flash:w:${hexfile}
