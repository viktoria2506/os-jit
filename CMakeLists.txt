cmake_minimum_required(VERSION 3.10)
project(jit)

set(CMAKE_CXX_STANDARD 14)

add_executable(jit jit_compiler.cpp)