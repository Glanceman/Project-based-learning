cmake_minimum_required(VERSION 3.20.0)

project(TinyRenderer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 23)

# Set the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist)

set(Test True)
if(Test)
    add_subdirectory(./test)
endif()

# Collect source files .cpp .h
aux_source_directory(${CMAKE_SOURCE_DIR}/src SOURCES)

#remove .cpp .h in lesson folder
# Remove .cpp and .h files in lesson folder
file(GLOB LESSON_SOURCES "${CMAKE_SOURCE_DIR}/src/lesson/*.cpp" "${CMAKE_SOURCE_DIR}/src/lesson/*.h")
list(REMOVE_ITEM SOURCES ${LESSON_SOURCES})

# Remove main.cpp from sources
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")


set(USE_MAIN True) #<---########################################################################################
if(NOT USE_MAIN)
    set(LESSON lesson5.cpp)

    # Find the chosen lesson file
    set(ENTRY_POINT "${CMAKE_SOURCE_DIR}/src/lesson/${LESSON}")
    if(NOT EXISTS ${ENTRY_POINT})
        #if not exist file then change main.cpp as starting point
        set(ENTRY_POINT "${CMAKE_SOURCE_DIR}/src/main.cpp")
    endif()
else()
    set(ENTRY_POINT "${CMAKE_SOURCE_DIR}/src/main.cpp")
endif()

# Print the chosen entry point
message("Using entry point: ${ENTRY_POINT}")

# Print the list of files collected
message("Sources: ${SOURCES}")

add_executable(${CMAKE_PROJECT_NAME} ${ENTRY_POINT} ${SOURCES})

# Ensure the output directory exists
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
