
# sqlite_orm_lite
sqlite orm, C++20, simple, and easy to use.

Features:
- Minimalist: Only 500 lines of code
- Modern C++20: Utilizes Template Metaprogramming (TMP) techniques, with no macros involved
- Intuitive: User-friendly and straightforward API
## Build
```bash
git clone https://github.com/faker2048/sqlite_orm_lite.git
cd sqlite_orm_lite
nix develop # to enter a development shell environment
mkdir build && cd build
cmake ..
make -j
ctest # to run tests
```

## Usage
```C++
struct MyCustomType {
  int id;
  std::string name;
  double height;

  auto sql_constructor() {
    return SqlConstructorBuilder()
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

  db_file.EnsureTable<MyCustomType>(); // This will create the table

  MyCustomType data = {1, "Alice", 1.70};
  db_file.Insert(data); // Insert data into the database

  auto retrieved = db_file.GetTable<MyCustomType>();
  EXPECT_EQ(retrieved.size(), 1);
  EXPECT_EQ(retrieved[0].id, data.id);
  EXPECT_EQ(retrieved[0].name, data.name);
  EXPECT_DOUBLE_EQ(retrieved[0].height, data.height);
}

TEST(SqliteFileTest, InsertRowsAndRetrieveData) {
  TmpDir tmp_dir{"InsertRowsAndRetrieveData"};
  SqliteFile db_file(tmp_dir.path() / "test.db");

  db_file.EnsureTable<MyCustomType>(); // Ensures that the table exists

  std::vector<MyCustomType> data = {
      {1, "Alice", 1.70}, {2, "Bob", 1.80}, {3, "Charlie", 1.90}};

  db_file.InsertRows(data); // Insert multiple rows

  auto retrieved = db_file.GetTable<MyCustomType>();
  EXPECT_EQ(retrieved.size(), data.size());
  for (int i = 0; i < data.size(); ++i) {
    EXPECT_EQ(retrieved[i].id, data[i].id);
    EXPECT_EQ(retrieved[i].name, data[i].name);
    EXPECT_DOUBLE_EQ(retrieved[i].height, data[i].height);
  }
}
```

## Code Standards
This project follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html). Adhering to these guidelines ensures that the codebase remains clean, consistent, and maintainable.

