#!/bin/bash
set -e

cmake --preset default
cmake --build --preset default

build/test/kl/kltests
