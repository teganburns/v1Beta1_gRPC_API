# Author: Tegan Burns
# Website: teganburns.com

cmake_minimum_required ( VERSION 3.8.2 )

# Use either one for DEBUGGING
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

PROJECT(streaming C CXX )
include(FindProtobuf)

set( VERSION_MAJOR 1 )
set( VERSION_MINOR 0 )


# Dirs
set ( STREAMING_BIN_DIR "${PROJECT_BINARY_DIR}/src/streaming" )
set ( STREAMING_SRC_DIR "${PROJECT_SOURCE_DIR}/src/streaming" )
#set ( cpu_dir "${PROJECT_BINARY_DIR}/src/cpu" )

# cpp
set ( STREAMING_CPP "${STREAMING_SRC_DIR}/streaming.cpp" )
#set ( cpu_cpp "${PROJECT_SOURCE_DIR}/src/cpu/cpu.cpp" )

# hpp
set ( STREAMING_HPP "${PROJECT_BINARY_DIR}/streaming.hpp" )
#set ( cpu_hpp "${PROJECT_BINARY_DIR}/src/cpu/cpu.hpp" )

# Proto
set( proto_include_dir "${PROJECT_SOURCE_DIR}/src/streaming" )


set(EXECUTABLE_NAME "streaming")

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED network audio graphics window system)


# REQUIRED
find_package( Protobuf REQUIRED )

# STATUS MESSAGES

message( STATUS "PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
message( STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

message( STATUS "STREAMING_CPP: ${STREAMING_CPP}")
message( STATUS "STREAMING_HPP: ${STREAMING_HPP}")
message( STATUS "protobuf_include_dirs: ${proto_include_dir}" )

message( STATUS "PROTOBUF_FOUND: ${PROTOBUF_FOUND}")
message( STATUS "PROTOBUF_PROTOC_LIBRARIES: ${PROTOBUF_PROTOC_LIBRARIES}")
message( STATUS "PROTOBUF_PROTOC_LIBRARY: ${PROTOBUF_PROTOC_LIBRARY}")
message( STATUS "PROTOBUF_PROTOC_EXECUTABLE: ${PROTOBUF_PROTOC_EXECUTABLE}")

# PROTOCOL BUFFERS
include_directories( ${proto_include_dir} )
protobuf_generate_cpp( PROTO_SRCS PROTO_HDRS ${proto_include_dir}/streaming.proto )
message( STATUS "PROTO_SRCS: ${PROTO_SRCS}")
message( STATUS "PROTO_HDRS: ${PROTO_HDRS}")

find_package(SFML 2 REQUIRED COMPONENTS network audio graphics window system)


# gRPC
# protoc -I ../src/streaming/ --grpc_out=. --plugin=protoc-gen-grpc=$( which grpc_cpp_plugin ) ../src/streaming/streaming.proto
# protoc -I ../src/streaming/ --grpc_out=. --plugin=protoc-gen-grpc=grpc/build/grpc_cpp_plugin ../src/streaming/streaming.proto
# protoc -I ../src/streaming/ --cpp_out=. ../src/streaming/streaming.proto



set(STREAMING_PROTO "${proto_include_dir}/streaming.proto")
set(GENERATED_PROTOBUF_PATH "${CMAKE_BINARY_DIR}")
set( STREAMING_PB_LIB "${GENERATED_PROTOBUF_PATH}/streaming.pb.o" )
set( STREAMING_GRPC_PB_LIB "${GENERATED_PROTOBUF_PATH}/streaming.grpc.pb.o" )

#file(MAKE_DIRECTORY ${GENERATED_PROTOBUF_PATH})
set(STREAMING_PB_CPP "${GENERATED_PROTOBUF_PATH}/streaming.pb.cc")
set(STREAMING_PB_H "${GENERATED_PROTOBUF_PATH}/streaming.pb.h")
set(STREAMING_GRPC_PB_CPP "${GENERATED_PROTOBUF_PATH}/streaming.grpc.pb.cc")
set(STREAMING_GRPC_PB_H "${GENERATED_PROTOBUF_PATH}/streaming.grpc.pb.h")

add_custom_command( 
    COMMENT "Building ${STREAMING_PB_H}" "${STREAMING_PB_CPP}"
    OUTPUT "${STREAMING_PB_H}" "${STREAMING_PB_CPP}"
    COMMAND "${PROJECT_BINARY_DIR}/grpc/build/third_party/protobuf/protoc"
    ARGS "-I ../src/streaming/ -I ./ --cpp_out=. ../src/streaming/streaming.proto"
)

add_custom_command(
    COMMENT "Building ${STREAMING_GRPC_PB_H}" "${STREAMING_GRPC_PB_CPP}"
    OUTPUT "${STREAMING_GRPC_PB_H}" "${STREAMING_GRPC_PB_CPP}"
    COMMAND "${PROJECT_BINARY_DIR}/grpc/build/third_party/protobuf/protoc"
    ARGS "-I ../src/streaming/ -I ./ --grpc_out=. --plugin=protoc-gen-grpc=grpc/build/grpc_cpp_plugin ../src/streaming/streaming.proto"
)


message( STATUS "STREAMING_PB_CPP: ${STREAMING_PB_CPP}" )
message( STATUS "STREAMING_PB_H: ${STREAMING_PB_H}" )
message( STATUS "STREAMING_GRPC_PB_CPP: ${STREAMING_GRPC_PB_CPP}" )
message( STATUS "STREAMING_GRPC_PB_H: ${STREAMING_GRPC_PB_H}" )
message( STATUS "GENERATED_PROTOBUF_FILES: ${GENERATED_PROTOBUF_FILES}" )

set( GENERATED_PROTOBUF_FILES ${STREAMING_PB_H_FILE} ${STREAMING_PB_CPP_FILE} ${STREAMING_GRPC_PB_H_FILE} ${STREAMING_GRPC_PB_CPP_FILE} )


#include(ExternalProject)
#
#set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/grpc_external)
#
#ExternalProject_Add(googletest
#    GIT_REPOSITORY https://github.com/grpc/grpc
#    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
#)
#
#include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
#link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

#add_executable(FirstTest ${streaming_cpp})
#add_dependencies(FirstTest googletest)
#target_link_libraries(FirstTest gtest gtest_main pthread)


include_directories( ${GENERATED_PROTOBUF_PATH} )
#link_libraries(grpc++_unsecure grpc gpr ${PROTOBUF_LIBRARY})


configure_file ( "${STREAMING_SRC_DIR}/streaming.hpp.in" ${STREAMING_HPP} )

include_directories (
    ${PROJECT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/grpc_external/include
    ${EXTERNAL_INSTALL_LOCATION}/include
)

link_directories (
    ${PROJECT_BINARY_DIR}
	${PROJECT_BINARY_DIR}/grpc_external/lib
    ${EXTERNAL_INSTALL_LOCATION}/lib
)

add_library( ${STREAMING_PB_LIB} ${STREAMING_PB_CPP} ${STREAMING_PB_H} ${PROTO_HDRS} )
add_library( ${STREAMING_GRPC_PB_LIB} ${STREAMING_GRPC_PB_CPP} ${STREAMING_GRPC_PB_H} ${PROTO_HDRS} )
add_executable( ${EXECUTABLE_NAME} ${STREAMING_CPP} ${STREAMING_HPP} ${PROTO_SRC} ${PROTO_HDRS} ${STREAMING_PB_H_FILE} ${STREAMING_GRPC_PB_H_FILE} )
target_link_libraries ( ${EXECUTABLE_NAME}  ${PROTOBUF_LIBRARIES} "streaming.pb.o" "streaming.grpc.pb.o" )


if(SFML_FOUND)
    include_directories( ${SFML_INCLUDE_DIR} )
    target_link_libraries( ${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} )
endif()

