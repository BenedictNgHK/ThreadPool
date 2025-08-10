cmake_minimum_required(VERSION 3.10)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

file(GLOB SRC ${PROJECT_SOURCE_DIR}/*.cpp)
include_directories(PUBLIC ${PROJECT_SOURCE_DIR})
add_executable(ThreadPool ${SRC})
