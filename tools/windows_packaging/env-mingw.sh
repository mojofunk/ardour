#!/bin/bash

: ${ARCH:="i686"}

: ${HOST:="${ARCH}-w64-mingw32"}

: ${MINGW_ROOT:="/usr/$HOST/sys-root/mingw"}

. env-product.sh

. env-tools.sh

. env-common.sh

. env-options.sh

if test -f $BUILD_CACHE_FILE
then
    . env-package.sh
fi
