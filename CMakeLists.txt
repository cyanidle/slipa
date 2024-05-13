cmake_minimum_required(VERSION 3.16)
project(test_slipa)

option(SLIPA_TEST "build tests" OFF)

add_library(slipa INTERFACE)
target_compile_features(slipa INTERFACE cxx_std_20)
target_include_directories(slipa INTERFACE .)

if (SLIPA_TEST)
    add_executable(slipa_test test.cpp)
    target_link_libraries(slipa PRIVATE slipa_test)
endif()
