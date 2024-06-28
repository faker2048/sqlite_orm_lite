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
  explicit SqliteFile(const std::filesystem::path& path) : path_(path) {
  }

  template <HasSqliteHelper T>
  void EnsureTable() {
    auto& sql = GetDefaultSqliteHelper<T>().GetEnsureTableSQLExpression();
    auto db   = sqlite3wrap::OpenDatabase(path_.c_str());
    sqlite3wrap::ExecuteSql(db.get(), sql);
  }

  template <HasSqliteHelper T>
  void DropTable() {
    auto& table_name = GetDefaultSqliteHelper<T>().GetTableName();
    std::string sql  = utils::StrCombine("DROP TABLE IF EXISTS \"", table_name, "\";");
    auto db          = sqlite3wrap::OpenDatabase(path_.c_str());
    sqlite3wrap::ExecuteSql(db.get(), sql);
  }

  template <HasSqliteHelper T>
  std::vector<T> GetTable() {
    std::vector<T> result;
    auto& helper    = GetDefaultSqliteHelper<T>();
    std::string sql = utils::StrCombine("SELECT * FROM \"", helper.GetTableName(), "\";");
    auto db         = sqlite3wrap::OpenDatabase(path_.c_str());
    sqlite3wrap::ExecuteSql(
        db.get(),
        sql,
        [](void* data, int argc, char** argv, char** col_name) {
          auto& result = *static_cast<std::vector<T>*>(data);
          T row;
          for (int i = 0; i < argc; ++i) {
            row.SetField(col_name[i], argv[i]);
          }
          result.push_back(row);
          return 0;
        },
        &result);
    return result;
  }

 private:
  std::filesystem::path path_;
};

}  // namespace sqliteol
