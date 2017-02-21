#!/usr/bin/env bash
set -ex

rm -rf build
mkdir build
cd build

CC_=${CC_:=$CC}
CXX_=${CXX_:=$CXX}

cmake -DCMAKE_C_COMPILER=$CC_ -DCMAKE_CXX_COMPILER=$CXX_ -DTESTS=ON ../
make VERBOSE=1

cd -
