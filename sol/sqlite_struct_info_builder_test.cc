#include "sol/sqlite_struct_info_builder.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace sqliteol;
using namespace testing;

TEST(SqliteStructInfoBuilderTest, BuildSqliteStructInfo) {
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

  auto sqlite_struct_info = MyCustomType(1001, "myname").sqlite_helper();
  EXPECT_EQ(sqlite_struct_info.GetEnsureTableSQLExpression(),
            "CREATE TABLE IF NOT EXISTS \"MyCustomType\"( id INT, name TEXT );");
  EXPECT_EQ(sqlite_struct_info.GenerateInsertSQLExpression(),
            "INSERT INTO \"MyCustomType\" ( id, name ) VALUES( 1001, myname );");
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
