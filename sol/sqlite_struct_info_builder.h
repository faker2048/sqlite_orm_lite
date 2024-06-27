#pragma once

#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "sol/serialize_template.h"
#include "sol/sqlite_struct_info.h"

namespace sqliteol {

template <typename RowTuple>
class SqliteStructInfo;

template <typename... CurColumnTypes>
class SqliteStructInfoBuilder {
 public:
  using CurRowTuple = std::tuple<CurColumnTypes...>;

  explicit SqliteStructInfoBuilder() {
  }

  template <typename... ColumnTypes>
  SqliteStructInfoBuilder(std::string&& table_name,
                          std::vector<std::string>&& column_names,
                          void* first_field_ref)
      : table_name_(std::move(table_name)),
        column_names_(std::move(column_names)),
        first_field_ref_(first_field_ref) {
  }

  SqliteStructInfoBuilder<CurColumnTypes...>& SetTableName(
      const std::string& table_name) {
    table_name_ = table_name;
    return *this;
  }

  template <typename ColumnType>
  SqliteStructInfoBuilder<CurColumnTypes..., ColumnType> AddColumn(
      std::string_view column_name, ColumnType* value) {
    column_names_.emplace_back(column_name);
    if constexpr (std::tuple_size_v<CurRowTuple> == 0) {
      first_field_ref_ = value;
    }
    return SqliteStructInfoBuilder<CurColumnTypes..., ColumnType>(
        std::move(table_name_), std::move(column_names_), first_field_ref_);
  }

  SqliteStructInfo<CurRowTuple> Build() {
    return SqliteStructInfo<CurRowTuple>(
        std::move(table_name_), std::move(column_names_), first_field_ref_);
  }

 private:
  // Although caching column_names_ could be used, but it is not necessary since the
  // database (file system) handles data storage anyway. So, let's keep the code simpler.
  std::string table_name_;
  std::vector<std::string> column_names_;
  void* first_field_ref_;
};

}  // namespace sqliteol