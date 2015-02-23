#!/bin/bash


. ../define_versions.sh

PRODUCT_NAME=${PRODUCT_NAME:=ardour}
PROGRAM_NAME=${PROGRAM_NAME:=Ardour}
PROGRAM_VERSION=${major_version}

LOWERCASE_DIRNAME=ardour3
# see wscript 'lwrcase_dirname' used for lib/ardour3 and share/ardour3
STATEFILE_SUFFIX=ardour # see filename_extensions.cc

# derived variables
PRODUCT_ID=${PROGRAM_NAME}${PROGRAM_VERSION}
PRODUCT_EXE=${PRODUCT_NAME}.exe
PRODUCT_ICON=${PRODUCT_NAME}.ico

# Figure out the Build Type
if [ x$DEBUG = xT ]; then
    PACKAGE_DIR="$PRODUCT_NAME-${release_version}-$ARCH-dbg"
else
    PACKAGE_DIR="$PRODUCT_NAME-${release_version}-$ARCH"
fi

PACKAGE_BIN_DIR=$PACKAGE_DIR/bin
PACKAGE_LIB_DIR=$PACKAGE_DIR/lib

if grep -q "BUILD_TESTS = True" $BUILD_CACHE_FILE; then
    WITH_TESTS=1
fi

ARDOUR_DATA_DIR=$PACKAGE_DIR/share/ardour3
