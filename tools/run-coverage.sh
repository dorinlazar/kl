#!/bin/bash
set -e

BUILD_DIR=build-coverage

rm -rf ${BUILD_DIR}
cmake --preset coverage
cmake --build --preset coverage

mkdir "${BUILD_DIR}/coverage"
lcov --ignore-errors mismatch --capture --no-external --initial --directory . -o "${BUILD_DIR}/coverage/zero-coverage.info"
"${BUILD_DIR}/test/kl/kltests"
lcov --ignore-errors mismatch --capture --no-external --directory .  --output-file "${BUILD_DIR}/coverage/kltests-coverage.info" 
lcov --ignore-errors mismatch --add-tracefile "${BUILD_DIR}/coverage/zero-coverage.info" --add-tracefile "${BUILD_DIR}/coverage/kltests-coverage.info" --output-file "${BUILD_DIR}/coverage/full-coverage.info"
lcov --ignore-errors mismatch --remove "${BUILD_DIR}/coverage/full-coverage.info" "${PWD}/test/*" --output-file "${BUILD_DIR}/coverage/coverage.info"
genhtml "${BUILD_DIR}/coverage/coverage.info" --output-directory "${BUILD_DIR}/coverage/"
