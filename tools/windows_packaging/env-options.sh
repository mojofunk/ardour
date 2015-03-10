#!/bin/bash

COMMON_OPTIONS=${COMMON_OPTIONS:="--noconfirm --use-external-libs --no-jack-metadata --dist-target=mingw --program-name=${PROGRAM_NAME}"}
PATH_OPTIONS=${PATH_OPTIONS:="--prefix=/ --configdir=/share"}
TEST_OPTIONS=${TEST_OPTIONS:="--test --single-tests"}
BACKEND_OPTIONS=${BACKEND_OPTIONS:="--with-backends=dummy,jack"}
