#!/usr/bin/env bash

CC=gcc cmake . -Bbuild -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release