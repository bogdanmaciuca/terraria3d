cmake_minimum_required(VERSION 3.15)
project(t3d LANGUAGES CXX C)

# Compiler flags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -O0 -g)
add_compile_definitions(GLFW_INCLUDE_NONE)

# Build directory (not necessary but matches your structure)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Source files
file(GLOB SRCS 
    src/*.cpp 
    vendor/simdjson/simdjson.cpp
)
file(GLOB C_SRCS
    vendor/glad/glad.c
    vendor/noise1234/noise1234.c
    vendor/lz4/lz4.c
)

# Include directories
include_directories(
    src
    vendor 
    vendor/glad
    vendor/simdjson
    vendor/noise1234
    vendor/lz4
)

# Find GLFW
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Create executable
add_executable(${PROJECT_NAME} ${SRCS} ${C_SRCS})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    glfw 
    OpenGL::GL
    ${CMAKE_DL_LIBS}  # For Linux DL library
)
