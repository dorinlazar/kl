#!/bin/bash

sudo dnf groupinstall -y "Development Tools"
sudo dnf install -y clang-tools-extra clang gcc-g++ ninja-build fmt-devel gtest-devel gmock-devel openssl-devel \
                    cppcheck valgrind lcov python3-devel pip cmake
pip install CodeChecker cmake-format
