#include "sol/sql_constructor_builder.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace sqliteol;
using namespace testing;

TEST(SqlConstructorBuilderTest, BuildSqlConstructor) {
  struct MyCustomType {
    int id;
    std::string name;

    auto sql_constructor() {
      return SqlConstructorBuilder<>()
          .SetTableName("MyCustomType")
          .AddColumn("id", &id)
          .AddColumn("name", &name)
          .Build();
    };
  };

  MyCustomType my_custom_type(111, "myname");
  auto sql_constructor = my_custom_type.sql_constructor();
  EXPECT_EQ(sql_constructor.GetEnsureTableSQL(),
            "CREATE TABLE IF NOT EXISTS \"MyCustomType\"( id INT, name TEXT );");
  EXPECT_EQ(sql_constructor.GetInsertSQL(),
            "INSERT INTO \"MyCustomType\" ( id, name ) VALUES( 111, 'myname' );");
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
