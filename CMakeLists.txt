cmake_minimum_required(VERSION 3.29.0)

project(AsChip8 VERSION 0.5 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

option(USE_CLANG_TIDY OFF)
if(USE_CLANG_TIDY)
  find_program(FOUND_CLANG_TIDY clang-tidy)
  if(FOUND_CLANG_TIDY)
    message(STATUS "Enabling clang-tidy")
    set (CMAKE_CXX_CLANG_TIDY clang-tidy;
      -checks=-*,cppcoreguidelines-*;
      -warnings-as-errors=*;
      )
  else()
    message(STATUS "Didn't find clang-tidy")
  endif()
endif()

option(USE_CPPCHECK OFF)
if(USE_CPPCHECK)
  find_program(FOUND_CPPCHECK cppcheck)
  if(FOUND_CPPCHECK)
    message(STATUS "Enabling cppcheck")
    set (CMAKE_CXX_CPPCHECK "${FOUND_CPPCHECK};--enable=all;--force;--std=c++20;--suppress=missingIncludeSystem")
  else()
    message(STATUS "Didn't find cppcheck")
  endif()
endif()

add_subdirectory(code)
