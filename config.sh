#!/bin/bash

callcmd=$0
confdir=`dirname $callcmd`
scriptname=${0##*/}

if [ $# -lt 1 ] ; then
    echo "Usage: ${scriptname} BoardName"
    echo
    exit 1
else
    board=$1
fi

toolchain=${confdir}/src/cmake/AvrGcc.cmake

if [ `uname` == "CYGWIN_NT-6.1" ] ; then
    cmake -G "NMake Makefiles" -D CMAKE_TOOLCHAIN_FILE=${toolchain} -D BOARD=${board} ${confdir}/src
else 
    cmake -D CMAKE_TOOLCHAIN_FILE=${toolchain} -D BOARD=${board} ${confdir}/src
fi

