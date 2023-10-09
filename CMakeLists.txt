cmake_minimum_required(VERSION 3.25)
set(LIBRARY_NAME kl)
project(${LIBRARY_NAME})

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
    include/${LIBRARY_NAME}/klflags.hpp
    include/${LIBRARY_NAME}/klfs.hpp
    include/${LIBRARY_NAME}/klexcept.hpp
    include/${LIBRARY_NAME}/kl.hpp
    include/${LIBRARY_NAME}/klio.hpp
    include/${LIBRARY_NAME}/klnet.hpp
    include/${LIBRARY_NAME}/klprocess.hpp
    include/${LIBRARY_NAME}/kltext.hpp
    include/${LIBRARY_NAME}/kltimecounter.hpp
    include/${LIBRARY_NAME}/kltime.hpp
    include/${LIBRARY_NAME}/klvalue.hpp
    include/${LIBRARY_NAME}/klds.hpp
    include/${LIBRARY_NAME}/klargparser.hpp
    include/${LIBRARY_NAME}/textscanner.hpp)

add_library(${LIBRARY_NAME} ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})

target_include_directories(
  ${LIBRARY_NAME}
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${LIBRARY_NAME}>
          $<INSTALL_INTERFACE:include/${LIBRARY_NAME}>)
target_include_directories(
  ${LIBRARY_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include>)

add_subdirectory(test)
