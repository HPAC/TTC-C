# ----------------------------------------------------------------------------
#  Root CMake file for TTC-TCC-C
#
#    From the off-tree build directory, invoke:
#      $ cmake <PATH_TO_TTC_TCC_C>
#
# ----------------------------------------------------------------------------
cmake_minimum_required (VERSION 2.8)
project (TTC_C)


# ----------------------------------------------------------------------------
# Build options
# ----------------------------------------------------------------------------
# Options
find_package(Doxygen)
option(BUILD_TEST "Generate executable test program.")
option(BUILD_DOC "Create the HTML based API documentation (requires doxygen)."
    ${DOXYGEN_FOUND})

# Variables
set(BUILD_TYPE "RELEASE" CACHE STRING "Build type, either DEBUG or RELEASE.")

# ----------------------------------------------------------------------------
# Compiler options
# ----------------------------------------------------------------------------
set(TTC_CC_FLAGS "-Wall -std=c99")
set(TTC_DEBUG_FLAG "-g -D TENSOR_DEBUG")
set(TTC_RELEASE_FLAG "-O2")

if (BUILD_TYPE STREQUAL "DEBUG")
    # Use debug configurations.
    set(TTC_CC_FLAGS "${TTC_CC_FALGS} ${TTC_DEBUG_FLAG}")
else ()
    # Use release configuration.
    set(TTC_CC_FLAGS "${TTC_CC_FALGS} ${TTC_RELEASE_FLAG}")
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TTC_CC_FLAGS}")

# ----------------------------------------------------------------------------
# Directory setting
# ----------------------------------------------------------------------------
# Add include files' directory
include_directories(include)
# Add source directory
add_subdirectory(include)
add_subdirectory(src)

# ----------------------------------------------------------------------------
# Build setting
# ----------------------------------------------------------------------------
if (BUILD_TEST)
    add_subdirectory(test)
endif ()

if (BUILD_DOC)
    add_subdirectory(doc)
endif()

