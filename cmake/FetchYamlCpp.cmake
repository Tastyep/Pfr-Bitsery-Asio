cmake_minimum_required(VERSION 3.24)

include(FetchContent)
set(FETCH_CONTENT_QUIET FALSE)

FetchContent_Declare(
  YamlCpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 0.8.0
  FIND_PACKAGE_ARGS NAMES YamlCpp)

FetchContent_MakeAvailable(YamlCpp)
