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
    return SqliteStructInfoBuilder<MyCustomType>()
        .SetTableName("MyCustomType")
        .AddColumn("id", &id)
        .AddColumn("name", &name)
        .AddColumn("height", &height)
        .Build();
  }
};

class SqliteFileTest : public Test {
 protected:
  TmpDir tmp_dir{"SqliteTestDir"};
  std::unique_ptr<SqliteFile> db_file;

  void SetUp() override {
    db_file = std::make_unique<SqliteFile>(tmp_dir.path() / "test.db");
    db_file->EnsureTable<MyCustomType>();
  }

  void TearDown() override {
    db_file->DropTable<MyCustomType>();
  }
};

// TEST_F(SqliteFileTest, TableCreationAndDeletion) {
//   // Test the creation and deletion of a table.
//   // The SetUp and TearDown already handle this, so we assume no exceptions mean success.
//   ASSERT_NO_THROW(db_file->EnsureTable<MyCustomType>());
//   ASSERT_NO_THROW(db_file->DropTable<MyCustomType>());
// }

// TEST_F(SqliteFileTest, InsertAndRetrieveData) {
//   MyCustomType data = {1, "Alice", 1.70};
//   ASSERT_NO_THROW(db_file->Insert(data));

//   auto retrievedData = db_file->GetTable<MyCustomType>();
//   ASSERT_EQ(retrievedData.size(), 1);
//   EXPECT_EQ(retrievedData[0].id, data.id);
//   EXPECT_EQ(retrievedData[0].name, data.name);
//   EXPECT_DOUBLE_EQ(retrievedData[0].height, data.height);
// }

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
