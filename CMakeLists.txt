cmake_minimum_required(VERSION 3.10)


# project name and version
project(RISC16EMU VERSION 1.0 LANGUAGES C CXX)

# Include directories if necessary
include_directories(${PROJECT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# assembler provided at https://user.eng.umd.edu/~blj/RiSC/a.c
add_executable(assembler a.c)

# RV-16 CPU emulator
add_executable(cpu RV-16.cpp)

