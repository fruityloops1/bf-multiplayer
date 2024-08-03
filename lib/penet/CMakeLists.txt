cmake_minimum_required(VERSION 3.5)
project(penet)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)

include_directories(Include)

file(GLOB_RECURSE SOURCES_CXX ${PROJECT_SOURCE_DIR}/Source/*.cpp)
add_library(penet ${SOURCES_CXX})