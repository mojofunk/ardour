#!/bin/bash

: ${ARCH:="i686"}

: ${HOST:="${ARCH}-w64-mingw32"}

: ${MINGW_ROOT:="/usr/$HOST/sys-root/mingw"}

. env-tools.sh

. env-common.sh

: ${APPNAME:="Ardour"}

# These are only relevant after a build
if test -f $BUILD_CACHE_FILE
then
        . ../define_versions.sh

	# Figure out the Build Type
	if [ x$DEBUG = xT ]; then
		PACKAGE_DIR="$APPNAME-${release_version}-$ARCH-dbg"
	else
		PACKAGE_DIR="$APPNAME-${release_version}-$ARCH"
	fi

	PACKAGE_BIN_DIR=$PACKAGE_DIR/bin
	PACKAGE_LIB_DIR=$PACKAGE_DIR/lib

	if grep -q "BUILD_TESTS = True" $BUILD_CACHE_FILE; then
		WITH_TESTS=1
	fi

	ARDOUR_DATA_DIR=$PACKAGE_DIR/share/ardour3
fi
