#include "sol/sqlite_file.h"

#include <filesystem>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "sol/sqlite_struct_info_builder.h"

using namespace sqliteol;
using namespace testing;

namespace {

class TmpDir {
 public:
  TmpDir(const std::string& name) {
    path_ = std::filesystem::temp_directory_path() / name;
    std::filesystem::create_directory(path_);
  }

  ~TmpDir() {
    std::filesystem::remove_all(path_);
  }

  const std::filesystem::path& path() const {
    return path_;
  }

 private:
  std::filesystem::path path_;
};

struct MyCustomType {
  int id;
  std::string name;
  double height;

  auto sqlite_helper() {
    return SqliteStructInfoBuilder<>()
        .SetTableName("MyCustomType")
        .AddColumn("id", &id)
        .AddColumn("name", &name)
        .AddColumn("height", &height)
        .Build();
  }
};

TEST(SqliteFileTest, InsertAndRetrieveData) {
  TmpDir tmp_dir{"InsertAndRetrieveData"};
  SqliteFile db_file(tmp_dir.path() / "test.db");

  db_file.EnsureTable<MyCustomType>();

  MyCustomType data = {1, "Alice", 1.70};
  db_file.Insert(data);

  auto retrieved = db_file.GetTable<MyCustomType>();
  ASSERT_EQ(retrieved.size(), 1);
  EXPECT_EQ(retrieved[0].id, data.id);
  EXPECT_EQ(retrieved[0].name, data.name);
  EXPECT_DOUBLE_EQ(retrieved[0].height, data.height);
}

TEST(SqliteFileTest, InsertRowsAndRetrieveData) {
  TmpDir tmp_dir{"InsertRowsAndRetrieveData"};
  SqliteFile db_file(tmp_dir.path() / "test.db");

  db_file.EnsureTable<MyCustomType>();

  std::vector<MyCustomType> data = {
      {1, "Alice", 1.70}, {2, "Bob", 1.80}, {3, "Charlie", 1.90}};

  db_file.InsertRows(data);

  auto retrieved = db_file.GetTable<MyCustomType>();
  ASSERT_EQ(retrieved.size(), data.size());
  for (int i = 0; i < data.size(); ++i) {
    EXPECT_EQ(retrieved[i].id, data[i].id);
    EXPECT_EQ(retrieved[i].name, data[i].name);
    EXPECT_DOUBLE_EQ(retrieved[i].height, data[i].height);
  }
}

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
