#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace sqliteol {

class SqlConstructorBuildCache {
 public:
  struct TableInfo {
    std::string table_name                                                 = "";
    std::vector<std::string> column_names                                  = {};
    std::string ensure_table_sql                                           = "";
    std::function<std::string(const void* first_field_ref)> insert_sql_gen = nullptr;
    std::unordered_map<std::string, int> column_name_to_index              = {};
    const std::type_info* row_tuple_type                                   = nullptr;
  };

  static SqlConstructorBuildCache& GetInstance() {
    static SqlConstructorBuildCache instance;
    return instance;
  }

  SqlConstructorBuildCache(const SqlConstructorBuildCache&)            = delete;
  SqlConstructorBuildCache& operator=(const SqlConstructorBuildCache&) = delete;

  std::optional<const TableInfo*> GetTableInfo(const std::string& table_name) const {
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
  std::unordered_map<std::string, TableInfo> cache_;  // table_name -> TableInfo
  mutable std::shared_mutex cache_mutex_;

 private:
  SqlConstructorBuildCache() = default;
};

}  // namespace sqliteol