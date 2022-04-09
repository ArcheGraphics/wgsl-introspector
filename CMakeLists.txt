cmake_minimum_required(VERSION 3.21)
project(wgsl_introspector)

set(CMAKE_CXX_STANDARD 17)

add_library(wgsl_introspector introspector.cpp wgsl_scanner.cpp wgsl_scanner.h)
