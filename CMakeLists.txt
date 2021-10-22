cmake_minimum_required(VERSION 3.16)
project(Parallel_programming)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(CommonFiles "function.cpp" "function.h" "utils.h")

find_package(Threads REQUIRED)

add_executable(Brute ${CommonFiles} "brute/brute.cpp")
add_executable(StdThread ${CommonFiles} "std_thread/std_thread.cpp")

IF (WIN32)
    add_executable(CreateThread ${CommonFiles} "create_thread/create_thread.cpp")

    add_executable(CreateProcess ${CommonFiles} "create_process/create_process.h" "create_process/create_process.cpp")
    target_link_libraries(CreateProcess ws2_32)
ENDIF ()

if (UNIX)
    enable_language(CUDA)

    add_executable(cuda ${CommonFiles} cuda/main.cu)

    set_target_properties(cuda PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

    SET(CMAKE_C_COMPILER mpicc)
    SET(CMAKE_CXX_COMPILER mpicxx)
    find_package(MPI REQUIRED)
    include_directories(${MPI_INCLUDE_PATH})

    add_executable(PthreadCreate ${CommonFiles} "pthread_create/pthread_create.cpp")
    target_link_libraries(PthreadCreate Threads::Threads)

    target_link_libraries(StdThread Threads::Threads)

    add_executable(Fork ${CommonFiles} fork/fork.h fork/fork.cpp)

    add_executable(MPI ${CommonFiles} "mpi/mpi.cpp" mpi/mpi.h)
    add_executable(MPIALL ${CommonFiles} mpi_all/mpi_all.cpp mpi_all/mpi_all.h)

    find_package(OpenMP REQUIRED)
    add_executable(OpenMP ${CommonFiles} omp/omp.cpp)
    target_link_libraries(OpenMP OpenMP::OpenMP_CXX)

endif (UNIX)
