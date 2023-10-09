#!/bin/bash
set -e

rm -rf build/
cmake --preset coverage
cmake --build --preset coverage
mkdir build/coverage
lcov --capture --no-external --initial --directory . -o build/coverage/zero-coverage.info
build/test/kl/kltests
lcov --capture --no-external --directory .  --output-file build/coverage/kltests-coverage.info 
lcov --add-tracefile build/coverage/zero-coverage.info --add-tracefile build/coverage/kltests-coverage.info --output-file build/coverage/full-coverage.info
lcov --remove build/coverage/full-coverage.info "${PWD}/test/*" --output-file build/coverage/coverage.info
genhtml build/coverage/coverage.info --output-directory build/coverage/
