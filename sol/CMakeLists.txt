add_subdirectory(utils)

sol_cc_gtest(
  NAME
    serialize_template_test
  SRCS
    "serialize_template_test.cc"
  DEPS
)

sol_cc_gtest(
  NAME
    sqlite_struct_info_builder_test
  SRCS
    "sqlite_struct_info_builder_test.cc"
  DEPS
)
