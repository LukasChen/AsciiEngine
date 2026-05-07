cmake_minimum_required(VERSION 3.10)
project(renderer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use GCC explicitly (optional: remove if you want CMake to pick the default compiler)
set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe")
set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe")

add_executable(renderer
    main.cpp
    model.cpp
    math3d.cpp
    camera.cpp
    renderer.cpp
)

# geometry.h and model.h are header-only, so just make sure their directory is included
target_include_directories(renderer PRIVATE ${CMAKE_SOURCE_DIR})

if(WIN32)
    # Windows console API is available by default, no extra libs needed
endif()

target_compile_options(renderer PRIVATE -Wall -Wextra -O3)
# target_link_options(renderer PRIVATE -s)
