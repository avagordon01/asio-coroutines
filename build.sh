#!/usr/bin/env bash

set -euxo pipefail

export CC=clang
export CXX=clang++

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix packaged
