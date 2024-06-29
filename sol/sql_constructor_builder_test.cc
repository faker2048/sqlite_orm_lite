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

  auto sql_constructor = MyCustomType().sql_constructor();
  EXPECT_EQ(sql_constructor.GetEnsureTableSQL(),
            "CREATE TABLE IF NOT EXISTS \"MyCustomType\"( id INTEGER, name TEXT );");
  EXPECT_EQ(sql_constructor.GetInsertSQL(),
            "INSERT INTO \"MyCustomType\" ( id, name ) VALUES( 0, );");
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
