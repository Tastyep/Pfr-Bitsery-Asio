cmake_minimum_required(VERSION 3.24)

include(FetchContent)
set(FETCH_CONTENT_QUIET FALSE)

FetchContent_Declare(
  Bitsery
  GIT_REPOSITORY https://github.com/fraillt/bitsery.git
  GIT_TAG v5.2.3
  FIND_PACKAGE_ARGS NAMES Bitsery)

FetchContent_MakeAvailable(Bitsery)
