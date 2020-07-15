cmake_minimum_required(VERSION 3.13)

project( code_sample LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 17 )

# You better believe it's required!
set( CMAKE_CXX_STANDARD_REQUIRED ON )

message( STATUS "cmake build type: ${CMAKE_BUILD_TYPE}")

# Compiler warnings should always be treated as errors. Untreated warnings lead to bugs.
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O3 -g")

option(BUILD_TESTS "Unit tests for sample code" ON)

# File to include external support stuff
include( ${CMAKE_SOURCE_DIR}/contrib/contrib.cmake )

# set components to compile as shared object files
set( components
     data
     operation )
     #execute ) # Maybe call this 1D operation?

foreach( component IN LISTS components )

  add_library( ${component} SHARED src/${component}.cpp )
  # All components are put in there
  target_include_directories( ${component} PRIVATE ${CMAKE_BINARY_DIR} )

endforeach()

# carry out the final linkage
add_executable( sample src/sample.cpp )

foreach( link IN LISTS components )

  target_link_libraries( sample PRIVATE ${link} )

endforeach()

# build the tests
if( BUILD_TESTS )

  enable_testing()

  add_library( tests_general SHARED testing/tests_general.cpp )
  set_target_properties( tests_general PROPERTIES LINKER_LANGUAGE CXX )
  target_link_libraries( tests_general PUBLIC Catch )

  foreach( component IN LISTS components )

    add_executable( ${component}_tests src/${component}_tests.cpp)
    target_include_directories(${component}_tests PRIVATE ${CMAKE_SOURCE_DIR}/testing)
    target_include_directories(${component}_tests PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(${component}_tests PRIVATE ${component} tests_general )

    # tests reflect the name of the executable they are given above
    add_test( NAME ${component}_test COMMAND ${component}_tests 
              WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

  endforeach()

else()

  message( WARNING "No unit tests built")

endif()
