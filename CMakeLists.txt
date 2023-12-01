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

set(LIBRARY_SOURCES
    src/klexcept.cpp
    src/klio.cpp
    src/klnet.cpp
    src/klprocess.cpp
    src/kltext.cpp
    src/kltimecounter.cpp
    src/kltime.cpp
    src/klvalue.cpp
    src/klfs.cpp
    src/klargparser.cpp
    src/textscanner.cpp)

set(LIBRARY_HEADERS
    include/kl/ds/array.hpp
    include/kl/ds/cursor.hpp
    include/kl/ds/dict.hpp
    include/kl/ds/list.hpp
    include/kl/ds/set.hpp
    include/kl/memory/unique_pointers.hpp
    include/kl/klflags.hpp
    include/kl/klfs.hpp
    include/kl/klexcept.hpp
    include/kl/kl.hpp
    include/kl/klio.hpp
    include/kl/klnet.hpp
    include/kl/klprocess.hpp
    include/kl/kltext.hpp
    include/kl/kltimecounter.hpp
    include/kl/kltime.hpp
    include/kl/memory.hpp
    include/kl/klvalue.hpp
    include/kl/klds.hpp
    include/kl/klargparser.hpp
    include/kl/textscanner.hpp)

add_library(kl ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})

target_include_directories(
  kl
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/kl>
          $<INSTALL_INTERFACE:include/kl>)
target_include_directories(
  kl PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include>)

add_subdirectory(test)
