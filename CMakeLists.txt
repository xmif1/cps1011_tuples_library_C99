cmake_minimum_required(VERSION 3.12)
project(tuple_t C)

set(CMAKE_C_STANDARD 99)

set(tuple_t_source tuple_t.c tuples_t.h)
add_library(tuple_t SHARED ${tuple_t_source})

add_executable(test_driver test_driver.c)
target_link_libraries(test_driver tuple_t)