project(shared)

set(${PROJECT_NAME}_SOURCES serialization/json/pretty_print.cpp)
list(TRANSFORM ${PROJECT_NAME}_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

message(STATUS "SRCS: ${${PROJECT_NAME}_SOURCES}")
find_package(Boost REQUIRED COMPONENTS pfr json)

include(${CMAKE_SOURCE_DIR}/cmake/FetchBitsery.cmake)
find_package(Bitsery)

include(${CMAKE_SOURCE_DIR}/cmake/FetchToml11.cmake)
find_package(toml11)

include(${CMAKE_SOURCE_DIR}/cmake/FetchYamlCpp.cmake)
find_package(yaml-cpp)

add_library(${PROJECT_NAME} SHARED)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
target_sources(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_SOURCES})
target_link_libraries(
  ${PROJECT_NAME} PUBLIC Boost::pfr Boost::json Bitsery::bitsery toml11::toml11
                         yaml-cpp::yaml-cpp Bitsery::bitsery)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})
