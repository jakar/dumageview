#
# dumageview listfile
#

cmake_minimum_required(VERSION 3.10)

project(dumageview)

# make release build, if not specified
# (from https://blog.kitware.com/cmake-and-the-default-build-type/)
set(default_build_type "RelWithDebInfo")

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Build type" FORCE)
  set_property(
    CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(data)
add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(thirdparty)
