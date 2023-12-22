cmake_minimum_required(VERSION 3.25)
project(kl)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

if(CMAKE_BUILD_TYPE MATCHES Release)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif(CMAKE_BUILD_TYPE MATCHES Release)

if(COVERAGE_ENABLED)
  set(CMAKE_CXX_FLAGS "-g -O0 --coverage -fprofile-arcs -ftest-coverage")
endif(COVERAGE_ENABLED)

set(LIBRARY_SOURCES src/except.cpp src/text.cpp)

set(LIBRARY_HEADERS
    include/kl/text.hpp
    include/kl/inttypes.hpp
    include/kl/memory/deleters.hpp
    include/kl/memory/unique_pointers.hpp
    include/kl/memory/ref_counted_pointer.hpp
    include/kl/except.hpp
    include/kl/memory.hpp)

add_library(kl ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})

target_include_directories(
  kl PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/kl>
             $<INSTALL_INTERFACE:include/kl>)
target_include_directories(
  kl PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)

add_subdirectory(test)
