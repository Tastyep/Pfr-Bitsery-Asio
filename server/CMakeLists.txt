cmake_minimum_required(VERSION 3.11)

project(server LANGUAGES CXX)

set(DEPS_BOOST_LIBRARIES pfr asio)
include(${CMAKE_SOURCE_DIR}/cmake/FetchBoost.cmake)
find_package(Boost REQUIRED COMPONENTS ${DEPS_BOOST_LIBRARIES})

set(${PROJECT_NAME}_SOURCES)
list(TRANSFORM ${PROJECT_NAME}_SOURCES PREPEND ${CMAKE_SOURCE_DIR}/src/)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp ${${PROJECT_NAME}_SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::pfr Boost::asio)
