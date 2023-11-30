add_executable(
  kltests kldate-tests.cpp klfs-tests.cpp kltext-tests.cpp
          kltextchain-tests.cpp kltextview-tests.cpp klvalue-tests.cpp klmemory.cpp)
target_link_libraries(kltests GTest::gtest_main kl)
target_include_directories(kltests PRIVATE ${CMAKE_SOURCE_DIR}/src)
include(GoogleTest)
gtest_discover_tests(kltests)
