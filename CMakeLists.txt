cmake_minimum_required(VERSION 3.10)
project(ThreadPoolTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
file(GLOB_RECURSE EXEC_SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB LIB_SRC ${PROJECT_SOURCE_DIR}/src/impl/*.cpp)
add_library(ThreadPool STATIC ${LIB_SRC})
# add_library(ThreadPool SHARED ${LIB_SRC})

add_executable(ThreadPoolTest ${EXEC_SRC})
