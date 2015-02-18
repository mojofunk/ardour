#!/bin/bash

. ./mingw-env.sh

export CC="distcc $HOST-gcc"
export CPP="distcc $HOST-g++"
export CXX="distcc $HOST-g++"

. ./env-options.sh
. ./print-env.sh

cd $BASE || exit 1
$PYTHON ./waf configure $COMMON_OPTIONS $PATH_OPTIONS \
                        $BACKEND_OPTIONS --optimize "$@"
