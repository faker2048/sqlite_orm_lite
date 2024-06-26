function(sol_cc_library)
  cmake_parse_arguments(
    PARSE_ARGV 0
    arg
    "PUBLIC"  # 单个选项
    "NAME"    # 单个值参数
    "HDRS;SRCS"  # 多值参数
  )

  # 创建库
  add_library(${arg_NAME} ${arg_SRCS})

  # 公共接口
  if(arg_PUBLIC)
    target_compile_definitions(${arg_NAME} INTERFACE SOL_LIBRARY_EXPORT)
  endif()
endfunction()


function(sol_cc_gtest)
  # 检查是否启用测试
  if(DISABLE_TESTING)
    return()
  endif()
  
  cmake_parse_arguments(
    PARSE_ARGV 0
    arg
    ""        # 无选项参数
    "NAME"    # 单个值参数
    "SRCS;DEPS"  # 多值参数
  )

  # 创建可执行文件
  add_executable(${arg_NAME} ${arg_SRCS})

  # 设置目标属性，确保输出到 tests/ 目录
  set_target_properties(${arg_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
  )

  # 添加依赖
  target_link_libraries(${arg_NAME} ${arg_DEPS} GTest::gmock GTest::gtest_main)
  gtest_discover_tests(${arg_NAME})

endfunction()
