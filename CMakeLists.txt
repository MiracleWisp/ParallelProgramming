cmake_minimum_required(VERSION 3.16)
project(Parallel_programming)

set(CMAKE_CXX_STANDARD 20)
set(CommonFiles "function.cpp" "function.h")

add_executable(Brute ${CommonFiles} "lab1/brute.cpp")

IF (WIN32)
    add_executable(CreateThread ${CommonFiles} lab2/create_thread.cpp)
ENDIF ()
