#pragma once

#include <filesystem>
#include <memory>

#include "sol/sqlite_struct_info_builder.h"
#include "sol/utils/str_utils.h"
#include "sqlite3.h"

namespace sqliteol {

namespace sqlite3wrap {

struct DbDeleter {
  void operator()(sqlite3* db) const {
    if (db) {
      sqlite3_close(db);
    }
  }
};

using DbPtr = std::unique_ptr<sqlite3, DbDeleter>;

DbPtr OpenDatabase(const char* filename) {
  sqlite3* db = nullptr;
  if (sqlite3_open(filename, &db) != SQLITE_OK) {
    throw std::runtime_error(
        utils::StrCombine("Failed to open database: ", sqlite3_errmsg(db)));
  }
  return DbPtr(db);
}

void ExecuteSql(sqlite3* db,
                const std::string& sql,
                int (*callback)(void*, int, char**, char**) = nullptr,
                void* data                                  = nullptr) {
  char* err_msg = nullptr;
  std::cout << sql << std::endl;
  if (sqlite3_exec(db, sql.c_str(), callback, data, &err_msg) != SQLITE_OK) {
    std::string error_message = "SQL execution failed: ";
    if (err_msg) {
      error_message += err_msg;
      sqlite3_free(err_msg);
    }
    throw std::runtime_error(error_message);
  }
}

}  // namespace sqlite3wrap

template <typename T>
concept HasSqliteHelper = requires { GetDefaultSqliteHelper<T>(); };

class SqliteFile {
 public:
  SqliteFile(const std::filesystem::path& path) : path_(path) {
  }

  template <HasSqliteHelper T>
  void EnsureTable() {
    const std::string& sql = GetDefaultSqliteHelper<T>().GetEnsureTableSQL();
    auto db                = sqlite3wrap::OpenDatabase(path_.c_str());
    sqlite3wrap::ExecuteSql(db.get(), sql);
  }

  template <HasSqliteHelper T>
  void DropTable() {
    std::string_view table_name = GetDefaultSqliteHelper<T>().GetTableName();
    std::string sql = utils::StrCombine("DROP TABLE IF EXISTS \"", table_name, "\";");
    auto db         = sqlite3wrap::OpenDatabase(path_.c_str());
    sqlite3wrap::ExecuteSql(db.get(), sql);
  }

  template <HasSqliteHelper T>
  std::vector<T> GetTable() {
    std::vector<T> result;
    auto& helper    = GetDefaultSqliteHelper<T>();
    std::string sql = utils::StrCombine("SELECT * FROM \"", helper.GetTableName(), "\";");

    T row;
    auto sqlite_helper = row.sqlite_helper();
    sqlite_helper.SetRef(&row);

    using CData = std::tuple<T*, decltype(sqlite_helper)*, std::vector<T>*>;
    CData data_to_sqlc{&row, &sqlite_helper, &result};
    constexpr int column_size = decltype(sqlite_helper)::column_size_;

    auto db = sqlite3wrap::OpenDatabase(path_.c_str());
    sqlite3wrap::ExecuteSql(
        db.get(),
        sql,
        [](void* data, int argc, char** argv, char** col_name) {
          auto [row, sqlite_helper, result] = *static_cast<CData*>(data);
          if (argc != column_size) {
            throw std::runtime_error("Column size mismatch");
          }

          magic::ForRange<0, column_size>(
              [&]<int I>() { sqlite_helper->template SetFieldByIndex<I>(argv[I]); });

          result->push_back(*row);
          return 0;
        },
        &data_to_sqlc);
    return result;
  }

  template <HasSqliteHelper T>
  void Insert(T& row) {
    auto helper     = row.sqlite_helper();
    std::string sql = helper.GetInsertSQL();

    auto db = sqlite3wrap::OpenDatabase(path_.c_str());
    sqlite3wrap::ExecuteSql(db.get(), sql.c_str());
  }

  template <HasSqliteHelper T>
  void InsertRows(std::vector<T>& rows, bool sync_off = false) {
    if (rows.empty()) {
      return;
    }
    std::vector<std::string> sqls;
    if (sync_off) {
      sqls.emplace_back("PRAGMA synchronous = OFF;");
    }
    sqls.emplace_back("BEGIN;");  // Open transaction
    auto helper = rows.front().sqlite_helper();
    for (auto& row : rows) {
      helper.SetRef(&row);
      sqls.emplace_back(helper.GetInsertSQL());
    }
    sqls.emplace_back("COMMIT;");

    auto db = sqlite3wrap::OpenDatabase(path_.c_str());
    sqlite3wrap::ExecuteSql(db.get(), utils::StrJoin("", sqls));
  }

 private:
  std::filesystem::path path_;
};

}  // namespace sqliteol
