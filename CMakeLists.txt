cmake_minimum_required(VERSION 3.16)
project(PODFlow)

set(CMAKE_CXX_STANDARD 17)

include_directories(./)
aux_source_directory(. DIR_SRCS)
aux_source_directory(./podflow DIR_SRCS)

MESSAGE(STATUS "executables: ${DIR_SRCS}")

add_executable(PODFlow ${DIR_SRCS})