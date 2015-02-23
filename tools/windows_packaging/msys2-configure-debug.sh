#!/bin/bash

. ./msys2-env.sh

. ./env-mingw.sh

export AR=$HOST-gcc-ar

. ./env-options.sh

. ./print-env.sh

cd $BASE || exit 1
$PYTHON ./waf configure $COMMON_OPTIONS $PATH_OPTIONS \
                        $TEST_OPTIONS $BACKEND_OPTIONS "$@"
