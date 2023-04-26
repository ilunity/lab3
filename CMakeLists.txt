cmake_minimum_required(VERSION 3.24)
project(lab3)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_EXE_LINKER_FLAGS "-static")


# Add MPI Package to Project
find_package(MPI REQUIRED)

add_executable(lab3 main.cpp sha256.cpp sha256.h)

# Add libraries for code completion and compiling
include_directories(${MSMPI_BIN})

target_link_libraries(lab3 PUBLIC MPI::MPI_CXX)
