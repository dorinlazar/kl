#!/bin/bash
set -e

cmake -S . -B build
cmake --build build -- -j 8
build/test/kl/kltests
