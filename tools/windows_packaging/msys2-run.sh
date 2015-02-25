#!/bin/bash

. ./env-mingw.sh

if [ ! -d $BASE/$PACKAGE_DIR ]; then
	echo "$PRODUCT_NAME must be packaged before running..."
    exit 1
fi

cd $BASE/$PACKAGE_DIR/bin || exit 1

./${PRODUCT_EXE} "$@"

