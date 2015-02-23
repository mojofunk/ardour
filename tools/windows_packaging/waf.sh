#!/bin/bash

. ./env-mingw.sh

cd $BASE || exit 1
$PYTHON ./waf "$@"
