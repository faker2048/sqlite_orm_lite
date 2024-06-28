#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "sol/sqlite_struct_info_builder.h"

using namespace sqliteol;
using namespace testing;

TEST(SqliteStructInfoTest, BuildSqliteStructInfo) {
  struct MyCustomType {
    int id;
    std::string name;
    double heigh;

    auto sqlite_helper() {
      return SqliteStructInfoBuilder<>()
          .SetTableName("MyCustomType")
          .AddColumn("id", &id)
          .AddColumn("name", &name)
          .AddColumn("heigh", &heigh)
          .Build();
    };
  };
  MyCustomType my_custom_type(1001, "myname", 180.5);
  auto sqlite_struct_info = my_custom_type.sqlite_helper();
  EXPECT_EQ(
      sqlite_struct_info.GetEnsureTableSQLExpression(),
      "CREATE TABLE IF NOT EXISTS \"MyCustomType\"( id INT, name TEXT, heigh REAL );");
  EXPECT_EQ(sqlite_struct_info.GenerateInsertSQLExpression(),
            "INSERT INTO \"MyCustomType\" ( id, name, heigh ) VALUES( 1001, myname, "
            "180.500000 );");
}

TEST(SqliteStructInfoTest, SetField) {
  struct MyCustomType {
    int id;
    std::string name;

    auto sqlite_helper() {
      return SqliteStructInfoBuilder<>()
          .SetTableName("MyCustomType")
          .AddColumn("id", &id)
          .AddColumn("name", &name)
          .Build();
    };
  };

  MyCustomType my_custom_type(1001, "myname");
  auto sqlite_struct_info = my_custom_type.sqlite_helper();
  EXPECT_EQ(sqlite_struct_info.GenerateInsertSQLExpression(),
            "INSERT INTO \"MyCustomType\" ( id, name ) VALUES( 1001, myname );");

  sqlite_struct_info.SetField("id", "1002");
  sqlite_struct_info.SetField("name", "newname");
  EXPECT_EQ(sqlite_struct_info.GenerateInsertSQLExpression(),
            "INSERT INTO \"MyCustomType\" ( id, name ) VALUES( 1002, newname );");
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
