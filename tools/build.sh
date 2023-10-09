#!/bin/bash
set -e

cmake -S . -B build
cmake --build build -- -j 3
