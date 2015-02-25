#!/bin/bash

. ./env-mingw.sh

if test ! -d $BASE/$PACKAGE_DIR; then
	echo "Package directory does not exist"
	exit 1
fi

cd $BASE/$PACKAGE_DIR/bin || exit 1
