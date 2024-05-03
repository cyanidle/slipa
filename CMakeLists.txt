cmake_minimum_required(VERSION 3.16)
project(test_slipa)

add_executable(slipa_test test.cpp)
target_compile_features(slipa_test PRIVATE cxx_std_20)
