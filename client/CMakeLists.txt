cmake_minimum_required(VERSION 3.11)

project(client LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS pfr asio)

set(${PROJECT_NAME}_SOURCES)
list(TRANSFORM ${PROJECT_NAME}_SOURCES PREPEND ${CMAKE_SOURCE_DIR}/src/)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp ${${PROJECT_NAME}_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::pfr Boost::asio shared)
