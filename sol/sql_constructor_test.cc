#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "sol/sql_constructor_builder.h"

using namespace sqliteol;
using namespace testing;

TEST(SqlConstructorTest, BuildSqlConstructor) {
  struct MyCustomType {
    int id;
    std::string name;
    double heigh;

    auto sql_constructor() {
      return SqlConstructorBuilder<>()
          .SetTableName("MyCustomType")
          .AddColumn("id", &id)
          .AddColumn("name", &name)
          .AddColumn("heigh", &heigh)
          .Build();
    };
  };
  MyCustomType my_custom_type(1001, "myname", 180.5);
  auto sql_constructor = my_custom_type.sql_constructor();
  EXPECT_EQ(
      sql_constructor.GetEnsureTableSQL(),
      "CREATE TABLE IF NOT EXISTS \"MyCustomType\"( id INT, name TEXT, heigh REAL );");
  EXPECT_EQ(sql_constructor.GetInsertSQL(),
            "INSERT INTO \"MyCustomType\" ( id, name, heigh ) VALUES( 1001, 'myname', "
            "180.500000 );");
}

TEST(SqlConstructorTest, SetField) {
  struct MyCustomType2 {
    int id;
    std::string name;

    auto sql_constructor() {
      return SqlConstructorBuilder<>()
          .SetTableName("MyCustomType2")
          .AddColumn("id", &id)
          .AddColumn("name", &name)
          .Build();
    };
  };

  MyCustomType2 my_custom_type(1001, "myname");
  auto sql_constructor = my_custom_type.sql_constructor();
  EXPECT_EQ(sql_constructor.GetInsertSQL(),
            "INSERT INTO \"MyCustomType2\" ( id, name ) VALUES( 1001, 'myname' );");

  sql_constructor.SetFieldByName("id", "1002");
  sql_constructor.SetFieldByName("name", "newname");
  EXPECT_EQ(sql_constructor.GetInsertSQL(),
            "INSERT INTO \"MyCustomType2\" ( id, name ) VALUES( 1002, 'newname' );");
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
