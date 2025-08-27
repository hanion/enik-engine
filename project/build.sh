#!/bin/env bash
set -euo pipefail

cd module

cmake -S . -B build -G Ninja \
	-DCMAKE_BUILD_TYPE=Debug \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	--no-warn-unused-cli

cmake --build build --config Debug --parallel

cp ./build/libmodule.so ./
echo "done"
