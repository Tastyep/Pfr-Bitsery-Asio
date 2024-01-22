cmake_minimum_required(VERSION 3.11)

project(server LANGUAGES CXX)

set(DEPS_BOOST_LIBRARIES pfr asio)
include(${CMAKE_SOURCE_DIR}/cmake/FetchBoost.cmake)
find_package(Boost REQUIRED COMPONENTS ${DEPS_BOOST_LIBRARIES})

include(${CMAKE_SOURCE_DIR}/cmake/FetchBitsery.cmake)
find_package(Bitsery)

include(${CMAKE_SOURCE_DIR}/cmake/FetchToml11.cmake)
find_package(toml11)

set(${PROJECT_NAME}_SOURCES)
list(TRANSFORM ${PROJECT_NAME}_SOURCES PREPEND ${CMAKE_SOURCE_DIR}/src/)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp ${${PROJECT_NAME}_SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::pfr Boost::asio
                                              Bitsery::bitsery toml11::toml11)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
