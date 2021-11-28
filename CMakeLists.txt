cmake_minimum_required(VERSION 3.0.0)
project(tcpArena VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "-pthread ")
SET(CMAKE_EXE_LINKER_FLAGS  " -luuid")
include_directories(include/)

add_executable(run_client src/client.cc)

add_executable(run_server src/server.cc)

include(CPack)
