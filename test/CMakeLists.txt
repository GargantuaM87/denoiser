add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE dns::libdns GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)
