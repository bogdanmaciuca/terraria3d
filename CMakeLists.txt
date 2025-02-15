cmake_minimum_required(VERSION 3.15)
project(t3d LANGUAGES CXX C)

# Compiler flags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -g)

# Build directory (not necessary but matches your structure)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Source files
file(GLOB SRCS 
    src/*.cpp 
    vendor/simdjson/simdjson.cpp
)
file(GLOB C_SRCS vendor/glad/glad.c)

# Include directories
include_directories(
    src
    vendor 
    vendor/glad
    vendor/simdjson
)

# Find GLFW
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# FastNoise2
set(FASTNOISE2_NOISETOOL OFF CACHE BOOL "Build Noise Tool" FORCE)
add_subdirectory(vendor/FastNoise2)

# Create executable
add_executable(${PROJECT_NAME} ${SRCS} ${C_SRCS})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    glfw 
    OpenGL::GL
    FastNoise
    ${CMAKE_DL_LIBS}  # For Linux DL library
)
