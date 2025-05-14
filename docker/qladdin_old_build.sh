#!/bin/bash

set -x

git clone -b master https://github.com/mkipnis/QuantLibAddin-Old qladdin_old
cd qladdin_old

mkdir -p build
cd build

cmake ..

cmake --build . --config Debug -v
