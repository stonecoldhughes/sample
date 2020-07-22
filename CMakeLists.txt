# Set high level project parameters
cmake_minimum_required(VERSION 3.13)

project( code_sample LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 17 )

set( CMAKE_CXX_STANDARD_REQUIRED ON )

message( STATUS "cmake build type: ${CMAKE_BUILD_TYPE}")

# Compiler warnings should always be treated as errors. Untreated warnings lead to bugs.
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O3 -g")

option(BUILD_TESTS "Unit tests for sample code" ON)

# File to include external support stuff
include( ${CMAKE_SOURCE_DIR}/contrib/contrib.cmake )

# The project is broken into separable, testable modules compiled separately as shared object
# files.
set( components
     data
     operation )

foreach( component IN LISTS components )

  add_library( ${component} SHARED src/${component}.cpp )
  target_include_directories( ${component} PRIVATE ${CMAKE_BINARY_DIR} )

endforeach()

# all components are compiled and linked to the final executable
add_executable( sample src/sample.cpp )

foreach( link IN LISTS components )

  target_link_libraries( sample PRIVATE ${link} )

endforeach()

# build the tests
if( BUILD_TESTS )

  enable_testing()

  add_library( tests_general SHARED testing/tests_general.hpp )
  set_target_properties( tests_general PROPERTIES LINKER_LANGUAGE CXX )
  target_link_libraries( tests_general PUBLIC Catch data )
  target_include_directories( tests_general PRIVATE ${CMAKE_SOURCE_DIR}/src )

  # Link each test file to the implementation it tests
  foreach( component IN LISTS components )

    add_executable( ${component}_tests src/${component}_tests.cpp)
    target_include_directories(${component}_tests PRIVATE ${CMAKE_SOURCE_DIR}/testing)
    target_include_directories(${component}_tests PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(${component}_tests PRIVATE ${component} tests_general )

    # tests reflect the name of the executable they are given above
    add_test( NAME ${component}_test COMMAND ${component}_tests 
              WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

  endforeach()

  # additional libraries
  target_link_libraries( operation_tests PRIVATE data )

else()

  message( WARNING "No unit tests built")

endif()
