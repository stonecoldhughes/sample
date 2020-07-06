# Support for external modules goes here


if( BUILD_TESTS )

  # Header-only library for testing interface
  add_library( Catch INTERFACE )
  target_include_directories( Catch INTERFACE
  ${CMAKE_SOURCE_DIR}/contrib/catch2/include )

endif()
