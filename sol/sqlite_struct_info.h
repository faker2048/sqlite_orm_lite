#pragma once

#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "sol/sqlite_struct_info_build_cache.h"
#include "sol/utils/str_utils.h"

namespace sqliteol {

template <typename RowTuple>
class SqliteStructInfo {
 public:
  using RowType = RowTuple;

  static constexpr size_t column_size_ = std::tuple_size_v<RowType>;

  template <size_t I>
  using ColumnType = std::tuple_element_t<I, RowType>;

  using TableInfo = SqliteStructInfoBuildCache::TableInfo;

  SqliteStructInfo(const TableInfo* kTableInfo, void* first_field_ref)
      : kTableInfo_(kTableInfo), first_field_ref_(first_field_ref) {
  }

  std::string GetEnsureTableSQLExpression() const {
    return kTableInfo_->ensure_table_sql;
  }

  std::string GenerateInsertSQL() const {
    return kTableInfo_->insert_sql_gen(first_field_ref_);
  }

  void SetField(const std::string& column_name, const std::string& value) const {
    magic::ForRange<0, column_size_>([&]<int I>() {
      if (column_name == kTableInfo_->column_names[I]) {
        *magic::GetFieldRef<RowTuple, I>(first_field_ref_) =
            FromDataBaseString<ColumnType<I>>(value);
      }
    });
  }

  std::string_view GetTableName() const {
    return kTableInfo_->table_name;
  }

  const std::vector<std::string>& GetColumnNames() const {
    return kTableInfo_->column_names;
  }

 private:
  const TableInfo* kTableInfo_;
  void* first_field_ref_;
};

template <typename T>
  requires requires(T x) { x.sqlite_helper(); }
const SqliteStructInfo<T>& GetDefaultSqliteHelper() {
  static SqliteStructInfo<T> kDefault = T{}.sqlite_helper();
  return kDefault;
}

}  // namespace sqliteol