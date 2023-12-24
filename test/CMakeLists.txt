enable_testing()
find_package(GTest REQUIRED)

set(TEST_SOURCES klarray.cpp klbasictext.cpp klmemory.cpp)

add_executable(kltests ${TEST_SOURCES})

target_link_libraries(kltests GTest::gtest_main kl)

target_include_directories(kltests PRIVATE ${CMAKE_SOURCE_DIR}/src)

include(GoogleTest)

gtest_discover_tests(kltests)
