cmake_minimum_required(VERSION 3.5)

project(notmond LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSON-C REQUIRED IMPORTED_TARGET json-c)
pkg_check_modules(LIBCURL REQUIRED IMPORTED_TARGET libcurl)

message(STATUS "GCC detected, adding compile flags")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -pthread -pedantic -Wall -Wextra")

add_executable(notmond
  support.c
  curl.c
  telebot.c
  config.c
  notmon.c
  main.c)

target_link_libraries(notmond PUBLIC PkgConfig::JSON-C PkgConfig::LIBCURL)
