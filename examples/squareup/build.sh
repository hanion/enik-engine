#!/bin/bash

cd module
cmake --no-warn-unused-cli \
	-DCMAKE_BUILD_TYPE:STRING=Debug \
	-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
	-S. -B./build -G Ninja
cmake --build ./build --config Debug

cp ./build/libmodule.so ./
echo "done"
