cmake_minimum_required(VERSION 3.13.0)

project(protoMessage VERSION 0.1.0 LANGUAGES C CXX)

#https://blog.csdn.net/qq_43190806/article/details/130210412
set(absl_DIR ${CMAKE_SOURCE_DIR}/libs/protobuf-v31.1-g++/lib/cmake/absl)
set(utf8_range_DIR ${CMAKE_SOURCE_DIR}/libs/protobuf-v31.1-g++/lib/cmake/utf8_range)
set(Protobuf_DIR ${CMAKE_SOURCE_DIR}/libs/protobuf-v31.1-g++/lib/cmake/protobuf)

find_package(Protobuf REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_CXX_FLAGS "-std=c++2a -Wall -Wextra -Werror -Wpedantic")
elseif(MSVC)
    add_compile_options(/W4)
endif()

# file(GLOB_RECURSE SOURCES
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
# )


# set the compiler
set(PROTO_COMPILER ${CMAKE_SOURCE_DIR}/libs/protobuf-v31.1-g++/bin/protoc.exe)
message(STATUS "PROTOBUF compiler ${PROTO_COMPILER}")

# Gather .proto files
file(GLOB_RECURSE PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/idl/*.proto")

# Generate C++ code from .proto files
set(GENERATED_SOURCES)
foreach(PROTO_FILE ${PROTO_FILES})
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
    set(GENERATED_CC "${CMAKE_CURRENT_SOURCE_DIR}/gen/${PROTO_NAME}.pb.cc")
    set(GENERATED_H "${CMAKE_CURRENT_SOURCE_DIR}/gen/${PROTO_NAME}.pb.h")
    
    add_custom_command(
        OUTPUT ${GENERATED_CC} ${GENERATED_H}
        COMMAND ${PROTO_COMPILER} --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/gen --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/idl ${PROTO_FILE}
        DEPENDS ${PROTO_FILE}
        COMMENT "Generating C++ code from ${PROTO_FILE}"
        VERBATIM
    )
    list(APPEND GENERATED_SOURCES ${GENERATED_CC})
endforeach()


#aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/gen/" SOURCES)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

#add_executable(protoMessage ${SOURCES} )
add_library(protoMessage STATIC ${GENERATED_SOURCES})

target_include_directories(protoMessage PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen    
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/libs/protobuf-v31.1-g++/include
)

#get_target_property(PROTOBUF_LIB_PATH protobuf::libprotobuf IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE)
#message(STATUS "protobuf::libprotobuf is located at: ${PROTOBUF_LIB_PATH}")

target_link_libraries(protoMessage protobuf::libprotobuf)


# # set(PROTO_SRCS)
# # set(PROTO_HDRS)
# foreach(IDL_FILE ${IDLSOURCES})
#     get_filename_component(IDL_NAME ${IDL_FILE} NAME_WE)
#     # Commented out code, consider removing if not needed
#     # set(OUTPUT_CC "${CMAKE_CURRENT_SOURCE_DIR}/gen/${IDL_NAME}.pb.cc")
#     # set(OUTPUT_H "${CMAKE_CURRENT_SOURCE_DIR}/gen/${IDL_NAME}.pb.h")

#     message(STATUS "IDL Name '${IDL_NAME}' at ${IDL_FILE}")
#     add_custom_command(
#         TARGET protoMessage PRE_BUILD
#         COMMAND ${PROTO_COMPILER} --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/gen --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/idl ${IDL_FILE}
#         COMMENT "Generating C++ code from ${IDL_FILE}"
#         VERBATIM
#     )
#     # Commented out code, consider removing if not needed
#     # list(APPEND PROTO_SRCS ${OUTPUT_CC})
#     # list(APPEND PROTO_HDRS ${OUTPUT_H})
# endforeach()

