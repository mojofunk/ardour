#!/bin/bash

. ./env-mingw.sh
. ./print-env.sh

cd $BASE || exit 1
$PYTHON ./waf configure $COMMON_OPTIONS $PATH_OPTIONS \
                        $BACKEND_OPTIONS --optimize "$@"
