cmake_minimum_required(VERSION 3.11)

project(netbench)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

set(BUILD_SHARED_LIBS ON)

set(DEPS_BOOST_LIBRARIES pfr asio json describe mp11)
include(${CMAKE_SOURCE_DIR}/cmake/FetchBoost.cmake)

add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(client)

file(CREATE_LINK "${CMAKE_BINARY_DIR}/compile_commands.json"
     "${CMAKE_SOURCE_DIR}/compile_commands.json" SYMBOLIC)
