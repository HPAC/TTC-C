# ----------------------------------------------------------------------------
#  Root CMake file for TTC-TCC-C
#
#    From the off-tree build directory, invoke:
#      $ cmake <PATH_TO_TTC_TCC_C>
#
# ----------------------------------------------------------------------------
cmake_minimum_required (VERSION 2.8)
project (TTC_TCC_C)


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
set(TENSOR_CC_FLAGS "-Wall -std=c99")
set(TENSOR_DEBUG_FLAG "-DTENSOR_DEBUG")
set(TENSOR_RELEASE_FLAG "-O3")

if (BUILD_TYPE STREQUAL "DEBUG")
    # Use debug configurations.
    set(TENSOR_CC_FLAGS "${TENSOR_CC_FALGS} ${TENSOR_DEBUG_FLAG}")
else ()
    # Use release configuration.
    set(TENSOR_CC_FLAGS "${TENSOR_CC_FALGS} ${TENSOR_RELEASE_FLAG}")
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TENSOR_CC_FLAGS}")

# ----------------------------------------------------------------------------
# Directory setting
# ----------------------------------------------------------------------------
# Add include files' directory
include_directories(include)
# Add source directory
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
