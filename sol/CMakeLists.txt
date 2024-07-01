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
    sql_constructor_builder_test
  SRCS
    "sql_constructor_builder_test.cc"
  DEPS
)

sol_cc_gtest(
  NAME
    sql_constructor_test
  SRCS
    "sql_constructor_test.cc"
  DEPS
)

sol_cc_gtest(
  NAME
    sqlite_file_test
  SRCS
    "sqlite_file_test.cc"
  DEPS
    sqlite3
)