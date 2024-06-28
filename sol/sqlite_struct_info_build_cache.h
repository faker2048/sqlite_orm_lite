#pragma once

#include <functional>
#include <iostream>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace sqliteol {

class SqliteStructInfoBuildCache {
 public:
  struct TableInfo {
    std::string table_name                = "";
    std::vector<std::string> column_names = {};
    std::string ensure_table_sql          = "";
    std::function<std::string(void* first_field_ref)> insert_sql_gen;
  };

  static SqliteStructInfoBuildCache& GetInstance() {
    static SqliteStructInfoBuildCache instance;
    return instance;
  }

  SqliteStructInfoBuildCache(const SqliteStructInfoBuildCache&)            = delete;
  SqliteStructInfoBuildCache& operator=(const SqliteStructInfoBuildCache&) = delete;

  std::optional<const TableInfo*> GetTableInfo(std::string_view table_name) const {
    std::shared_lock lock(cache_mutex_);
    auto it = cache_.find(table_name);
    if (it == cache_.end()) {
      return std::nullopt;
    }
    return &it->second;
  }

  bool AddTableInfo(TableInfo&& table_info) {
    if (GetTableInfo(table_info.table_name).has_value()) {
      return false;
    }
    std::unique_lock lock(cache_mutex_);
    std::string table_name = table_info.table_name;
    cache_[table_name]     = std::move(table_info);
    return true;
  }

 private:
  std::unordered_map<std::string_view, TableInfo> cache_;  // table_name -> TableInfo
  mutable std::shared_mutex cache_mutex_;

 private:
  SqliteStructInfoBuildCache() = default;

  // template <typename RowTuple>
  // std::string GetEnsureTableSql() const {
  //   constexpr size_t column_size         = std::tuple_size_v<RowTuple>;
  //   std::vector<std::string> column_spec = {};
  //   magic::ForRange<0, column_size>([&]<int I>() {
  //     column_spec.push_back(
  //         utils::StrCombine(column_names_[I], " ", SqliteColumnTypeStr_v<I>));
  //   });
  //   return utils::StrCombine("CREATE TABLE IF NOT EXISTS \"",
  //                            table_name_,
  //                            "\"( ",
  //                            utils::StrJoin(", ", column_spec),
  //                            " );");
  // }

  // template <typename RowTuple>
  // std::string GenerateInsertSQLExpression() const {
  //   constexpr size_t column_size           = std::tuple_size_v<RowTuple>;
  //   std::vector<std::string> column_names  = {};
  //   std::vector<std::string> column_values = {};
  //   magic::ForRange<0, column_size_>([&]<int I>() {
  //     column_names.push_back(column_names_[I]);
  //     column_values.push_back(
  //         ToDataBaseString(*magic::GetFieldRef<RowTuple, I>(first_field_ref_)));
  //   });

  //   return utils::StrCombine("INSERT INTO \"",
  //                            table_name_,
  //                            "\" ( ",
  //                            utils::StrJoin(", ", column_names),
  //                            " ) VALUES( ",
  //                            utils::StrJoin(", ", column_values),
  //                            " );");
  // }
};

}  // namespace sqliteol