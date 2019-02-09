#!/usr/bin/env bash

CC=gcc cmake . -Bbuild -GNinja -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug