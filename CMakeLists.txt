cmake_minimum_required(VERSION 3.14)

project(sqlite_orm_lite VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake") # Make cmake find our custom modules
include(SolCMakeFunctions)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(GTest REQUIRED)
enable_testing()

add_subdirectory(sol)

# install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sol/
  DESTINATION include/sol
  FILES_MATCHING PATTERN "*.h")

include(CMakePackageConfigHelpers)

# create a config version file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/sqlite_orm_liteConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# install config and config version files
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/sqlite_orm_liteConfigVersion.cmake"
  DESTINATION lib/cmake/sqlite_orm_lite)
