cmake_minimum_required(VERSION 3.13.0)

project(TinyRenderer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist)

# Collect source files
aux_source_directory(${CMAKE_SOURCE_DIR}/src SOURCES)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

# Ensure the output directory exists
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
