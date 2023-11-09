cmake_minimum_required(VERSION 3.26)
project(fridge_injector)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/**)

add_executable(fridge_injector ${SOURCES})
