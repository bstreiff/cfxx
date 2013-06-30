#=================================
# CMake configuration
#=================================
cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules" )

#=================================
# Project information
#=================================
project( CFXX C CXX )
set( CFXX_VERSION_MAJOR 0 )
set( CFXX_VERSION_MINOR 1 )

# setup where we dump executables, static/import libraries, and shared
# libraries/DLLs by default.
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CFXX_BINARY_DIR}/bin" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CFXX_BINARY_DIR}/lib" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CFXX_BINARY_DIR}/bin" )

# We want to use C++11 and libc++ for everything.
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

# gtest only wants <tr1/tuple>... but libc++ only offers it at <tuple>,
# and gtest doesn't know how to find that.
add_definitions("-DGTEST_HAS_TR1_TUPLE=0")

#=================================
# Add vendor components
#=================================

add_subdirectory( vendor/gtest-1.6.0 )

#=================================
# Add component tests
#=================================
add_subdirectory( tests )
