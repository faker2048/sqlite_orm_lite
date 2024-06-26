



# sol_cc_gtest(
#   NAME
#     serialize_base_test
#   SRCS
#     "serialize_base_test.cc"
#   DEPS
# )


function(sol_cc_library)
  cmake_parse_arguments(
    PARSE_ARGV 0
    arg
    "PUBLIC"  # Option to mark library for public interface usage
    "NAME"    # Single-value parameter: the name of the library
    "HDRS;SRCS"  # Multi-value parameters: headers and sources of the library
  )

  # Create the library with specified sources
  add_library(${arg_NAME} ${arg_SRCS})

  # Apply public interface settings
  if(arg_PUBLIC)
    target_compile_definitions(${arg_NAME} INTERFACE SOL_LIBRARY_EXPORT)
  endif()
endfunction()

function(sol_cc_gtest)
  # Check if testing is disabled
  if(DISABLE_TESTING)
    return()
  endif()
  
  cmake_parse_arguments(
    PARSE_ARGV 0
    arg
    ""        # No options
    "NAME"    # Single-value parameter: the name of the executable
    "SRCS;DEPS"  # Multi-value parameters: sources and dependencies of the executable
  )

  # Create the executable
  add_executable(${arg_NAME} ${arg_SRCS})

  # Set properties for the target to ensure output is in the 'tests/' directory
  set_target_properties(${arg_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
  )

  # Link dependencies and setup testing
  target_link_libraries(${arg_NAME} ${arg_DEPS} GTest::gmock GTest::gtest_main)
  gtest_discover_tests(${arg_NAME})
endfunction()
