cmake_minimum_required(VERSION 3.10)
project( IP )

if (APPLE)
    find_library(GECODE Gecode)
else()
    # Add the place of the find module to the module path
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
    # Find Gecode
    find_package(Gecode)
endif()

# C++ standard version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# The version number.
set (HC_VERSION_MAJOR 0)
set (HC_VERSION_MINOR 1)
set (HC_VERSION_PATCH 0)

if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()

# add the binary tree to the search path for include files
# so that we will find config.h
include_directories("${PROJECT_BINARY_DIR}/src/.")
include_directories ("${PROJECT_SOURCE_DIR}/src/.")
if (APPLE)
    include_directories("${GECODE}")
else()
    include_directories("${GECODE_INCLUDE_DIRS}")
endif()

add_subdirectory (src)
add_subdirectory (test)
