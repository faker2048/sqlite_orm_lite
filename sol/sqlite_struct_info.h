#pragma once

#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "sol/utils/str_utils.h"

namespace sqliteol {

template <typename RowTuple>
class SqliteStructInfo {
 public:
  using RowType = RowTuple;

  static constexpr size_t column_size_ = std::tuple_size_v<RowType>;

  template <size_t I>
  using ColumnType = std::tuple_element_t<I, RowType>;

  template <size_t I>
  static constexpr std::string_view SqliteColumnTypeStr_v =
      ToDataBaseType<ColumnType<I>>();

  SqliteStructInfo(std::string&& table_name,
                   std::vector<std::string>&& column_names,
                   void* first_field_ref)
      : table_name_(table_name),
        column_names_(column_names),
        first_field_ref_(first_field_ref) {
  }

  std::string GetEnsureTableSQLExpression() const {
    std::vector<std::string> column_spec = {};
    magic::ForRange<0, column_size_>([&]<int I>() {
      column_spec.push_back(
          utils::StrCombine(column_names_[I], " ", SqliteColumnTypeStr_v<I>));
    });

    return utils::StrCombine("CREATE TABLE IF NOT EXISTS \"",
                             table_name_,
                             "\"( ",
                             utils::StrJoin(", ", column_spec),
                             " );");
  }

  std::string GenerateInsertSQLExpression() const {
    std::vector<std::string> column_names  = {};
    std::vector<std::string> column_values = {};
    magic::ForRange<0, column_size_>([&]<int I>() {
      column_names.push_back(column_names_[I]);
      column_values.push_back(
          ToDataBaseString(*magic::GetFieldRef<RowTuple, I>(first_field_ref_)));
    });

    return utils::StrCombine("INSERT INTO \"",
                             table_name_,
                             "\" ( ",
                             utils::StrJoin(", ", column_names),
                             " ) VALUES( ",
                             utils::StrJoin(", ", column_values),
                             " );");
  }

  void SetField(const std::string& column_name, const std::string& value) const {
    magic::ForRange<0, column_size_>([&]<int I>() {
      if (column_name == column_names_[I]) {
        *magic::GetFieldRef<RowTuple, I>(first_field_ref_) =
            FromDataBaseString<ColumnType<I>>(value);
      }
    });
  }

 private:
  // Although caching could be used, but it is not necessary since the
  // database (file system) handles data storage anyway. So, let's keep the code simpler.
  std::string table_name_;
  std::vector<std::string> column_names_;
  void* first_field_ref_;
};

}  // namespace sqliteol