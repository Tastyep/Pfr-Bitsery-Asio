cmake_minimum_required(VERSION 3.24)

include(FetchContent)
set(FETCH_CONTENT_QUIET FALSE)

FetchContent_Declare(
  Toml11
  GIT_REPOSITORY https://github.com/ToruNiina/toml11.git
  GIT_TAG v3.8.1
  FIND_PACKAGE_ARGS NAMES Toml11)

FetchContent_MakeAvailable(Toml11)
