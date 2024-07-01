#pragma once

#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "sol/sql_constructor_build_cache.h"
#include "sol/utils/str_utils.h"

namespace sqliteol {

template <typename RowTuple>
class SqlConstructor {
 public:
  using RowType = RowTuple;

  static constexpr size_t column_size_ = std::tuple_size_v<RowType>;

  template <size_t I>
  using ColumnType = std::tuple_element_t<I, RowType>;

  using TableInfo = SqlConstructorBuildCache::TableInfo;

  inline SqlConstructor(const TableInfo* kTableInfo, void* first_field_ref)
      : kTableInfo_(kTableInfo), first_field_ref_(first_field_ref) {
  }

  inline void SetRef(void* first_field_ref) {
    first_field_ref_ = first_field_ref;
  }

  inline const std::string& GetEnsureTableSQL() const {
    return kTableInfo_->ensure_table_sql;
  }

  inline std::string GetInsertSQL() const {
    return kTableInfo_->insert_sql_gen(first_field_ref_);
  }

  inline void SetFieldByName(const std::string& column_name,
                             const std::string& value) const {
    magic::ForRange<0, column_size_>([&]<int I>() {
      if (column_name == kTableInfo_->column_names[I]) {
        *magic::GetAlignedRefByIndex<RowTuple, I>(first_field_ref_) =
            FromDataBaseString<ColumnType<I>>(value);
      }
    });
  }

  template <int I>
  void SetFieldByIndex(const std::string& value) const {
    static_assert(I >= 0 && I < column_size_, "Index out of range");
    if (!first_field_ref_) {
      throw std::runtime_error("first_field_ref_ is nullptr");
    }
    *magic::GetAlignedRefByIndex<RowTuple, I>(first_field_ref_) =
        FromDataBaseString<ColumnType<I>>(value);
  }

  inline std::string_view GetTableName() const {
    return kTableInfo_->table_name;
  }

  inline const std::vector<std::string>& GetColumnNames() const {
    return kTableInfo_->column_names;
  }

 private:
  const TableInfo* kTableInfo_;
  void* first_field_ref_;
};

template <typename T>
  requires requires(T x) { x.sql_constructor(); }
const auto& GetDefaultSqliteHelper() {
  static auto kDefault = T{}.sql_constructor();
  return kDefault;
}

}  // namespace sqliteol