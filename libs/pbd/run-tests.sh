#!/bin/bash

SCRIPTPATH=$( cd $(dirname $0) ; pwd -P )
TOP="$SCRIPTPATH/../.."
PBD_LIBS_DIR=$TOP/build/libs/pbd

. $TOP/build/gtk2_ardour/ardev_common_waf.sh

cd $PBD_LIBS_DIR

if [ "$1" == "--debug" ]
then
        gdb ./run-tests $*
elif [ "$1" == "--valgrind" ]
then
        valgrind --tool="memcheck" ./run-tests
else
        ./run-tests $*
fi
