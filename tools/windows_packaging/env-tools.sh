#!/bin/bash
# export necessary env vars, setting defaults if not already set.

export PKG_CONFIG_PREFIX=${PKG_CONFIG_PREFIX:=$MINGW_ROOT}
export PKG_CONFIG_LIBDIR=${PKG_CONFIG_LIBDIR:=$MINGW_ROOT/lib/pkgconfig}
export PKGCONFIG=${PKGCONFIG:=pkg-config}
export AR=${AR:=$HOST-ar}
export RANLIB=${RANLIB:=$HOST-ranlib}
export CC=${CC:=$HOST-gcc}
export CPP=${CPP:=$HOST-g++}
export CXX=${CXX:=$HOST-g++}
export AS=${AS:=$HOST-as}
export LINK_CC=${LINK_CC:=$HOST-gcc}
export LINK_CXX=${LINK_CXX:=$HOST-g++}
export WINRC=${WINRC:=$HOST-windres}
export STRIP=${STRIP:=$HOST-strip}
export PYTHON=${PYTHON:=python}
