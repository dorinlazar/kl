#!/bin/bash

sudo dnf install -y clang-tools-extra clang gcc-g++ ninja-build fmt-devel gtest-devel gmock-devel openssl-devel \
                    cppcheck valgrind lcov python3-devel pip
pip install CodeChecker cmake-format
