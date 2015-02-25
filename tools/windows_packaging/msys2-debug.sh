#!/bin/bash

. ./env-mingw.sh

if [ ! -d $BASE/$PACKAGE_DIR ]; then
	echo "$PRODUCT_NAME must be packaged before debugging..."
    exit 1
fi

# needed for msys2 gdb
export PYTHONHOME=/mingw32

cd $BASE/$PACKAGE_DIR || exit 1

./debug.bat

