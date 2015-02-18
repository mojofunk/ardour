#!/bin/bash

. ./mingw-env.sh
. ./env-options.sh
. ./print-env.sh

cd $BASE || exit 1
$PYTHON ./waf configure $COMMON_OPTIONS $PATH_OPTIONS \
                        $TEST_OPTIONS $BACKEND_OPTIONS "$@"
