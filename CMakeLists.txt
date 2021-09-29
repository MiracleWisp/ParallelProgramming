cmake_minimum_required(VERSION 3.16)
project(Parallel_programming)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(CommonFiles "function.cpp" "function.h" "utils.h")

find_package(Threads REQUIRED)

add_executable(Brute ${CommonFiles} "brute/brute.cpp")

IF (WIN32)
    add_executable(CreateThread ${CommonFiles} create_thread/create_thread.cpp)
    add_executable(CreateProcess ${CommonFiles} create_process/create_process.cpp)
    target_link_libraries(CreateProcess ws2_32)
ENDIF ()

if (UNIX)
    add_executable(PthreadCreate ${CommonFiles} pthread_create/pthread_create.cpp)
    target_link_libraries(PthreadCreate Threads::Threads)
endif (UNIX)
