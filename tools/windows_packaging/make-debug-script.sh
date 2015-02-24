#! /bin/bash

echo "Creating debug.bat"
cat > $PACKAGE_DIR/debug.bat << EOF
cd bin
START gdb.exe -iex "dir ../src/gtk2_ardour" -iex "set logging overwrite on" -iex "set height 0" -iex "set logging on %UserProfile%\\${PRODUCT_NAME}-debug.log" ${PRODUCT_EXE}
EOF

