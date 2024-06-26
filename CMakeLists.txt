cmake_minimum_required(VERSION 3.10)

project(sqlite_orm_lite)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake") # Make cmake find our custom modules
include(SolCMakeFunctions)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(GTest REQUIRED)
enable_testing()

add_subdirectory(sol)