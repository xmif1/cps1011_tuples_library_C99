cmake_minimum_required(VERSION 3.12)
project(tuple_t C)

set(CMAKE_C_STANDARD 99)

set(tuple_t tuple_t.c tuples_t.h)
add_library(tuples_t SHARED ${tuple_t})

set(test_driver test_driver.c)
add_executable(test_driver ${test_driver})
target_link_libraries(test_driver tuple_t)