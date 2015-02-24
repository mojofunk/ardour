#!/bin/bash

. ./env-mingw.sh

export CC="distcc $HOST-gcc"
export CPP="distcc $HOST-g++"
export CXX="distcc $HOST-g++"

. ./print-env.sh

cd $BASE || exit 1
$PYTHON ./waf configure $COMMON_OPTIONS $PATH_OPTIONS \
                        $TEST_OPTIONS $BACKEND_OPTIONS "$@"
