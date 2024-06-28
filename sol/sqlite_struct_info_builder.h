#pragma once

#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "sol/serialize_template.h"
#include "sol/sqlite_struct_info.h"
#include "sol/sqlite_struct_info_build_cache.h"

namespace sqliteol {

template <typename RowTuple>
class SqliteStructInfo;

template <typename... CurColumnTypes>
class SqliteStructInfoBuilder {
 public:
  using CurRowTuple = std::tuple<CurColumnTypes...>;
  using BuildCache  = SqliteStructInfoBuildCache;
  using TableInfo   = BuildCache::TableInfo;

  template <size_t I>
  static constexpr std::string_view SqliteColumnTypeStr_v =
      ToDataBaseType<std::tuple_element_t<I, CurRowTuple>>();

  explicit SqliteStructInfoBuilder() {
  }

  template <typename... ColumnTypes>
  SqliteStructInfoBuilder(std::unique_ptr<TableInfo>&& tmp_, void* first_field_ref)
      : tmp_(std::move(tmp_)), first_field_ref_(first_field_ref) {
  }

  SqliteStructInfoBuilder<CurColumnTypes...>& SetTableName(std::string_view table_name) {
    std::optional<const TableInfo*> cached =
        BuildCache::GetInstance().GetTableInfo(table_name);

    if (cached.has_value()) {
      kTableInfo_ = cached.value();
    } else {
      tmp_             = std::make_unique<TableInfo>();
      tmp_->table_name = table_name;
    }
    return *this;
  }

  template <typename ColumnType>
  SqliteStructInfoBuilder<CurColumnTypes..., ColumnType> AddColumn(
      std::string_view column_name, ColumnType* value) {
    if constexpr (std::tuple_size_v<CurRowTuple> == 0) {
      first_field_ref_ = value;
    }

    if (!is_built()) {
      tmp_->column_names.emplace_back(column_name);
    }
    return SqliteStructInfoBuilder<CurColumnTypes..., ColumnType>(std::move(tmp_),
                                                                  first_field_ref_);
  }

  SqliteStructInfo<CurRowTuple> Build() {
    if (!is_built()) {
      tmp_->ensure_table_sql = GetEnsureTableSql<CurRowTuple>();
      tmp_->insert_sql_gen   = GenerateInsertSQLExpressionFunc<CurRowTuple>();

      std::string table_name = tmp_->table_name;
      bool ok                = BuildCache::GetInstance().AddTableInfo(std::move(*tmp_));

      kTableInfo_ = BuildCache::GetInstance().GetTableInfo(table_name).value();
    }

    return SqliteStructInfo<CurRowTuple>(kTableInfo_, first_field_ref_);
  }

 private:
  inline bool is_built() const {
    return kTableInfo_ != nullptr;
  }

  template <typename RowTuple>
  std::string GetEnsureTableSql() const {
    constexpr size_t column_size = std::tuple_size_v<RowTuple>;

    std::vector<std::string> column_spec = {};
    magic::ForRange<0, column_size>([&]<int I>() {
      column_spec.push_back(
          utils::StrCombine(tmp_->column_names[I], " ", SqliteColumnTypeStr_v<I>));
    });

    return utils::StrCombine("CREATE TABLE IF NOT EXISTS \"",
                             tmp_->table_name,
                             "\"( ",
                             utils::StrJoin(", ", column_spec),
                             " );");
  }

  template <typename RowTuple>
  auto GenerateInsertSQLExpressionFunc() const {
    constexpr size_t column_size    = std::tuple_size_v<RowTuple>;
    std::string column_names_joined = utils::StrJoin(", ", tmp_->column_names);

    auto f = [table_name = tmp_->table_name, column_names_joined](void* first_field_ref) {
      std::vector<std::string> column_values = {};
      magic::ForRange<0, column_size>([&]<int I>() {
        column_values.push_back(
            ToDataBaseString(*magic::GetFieldRef<RowTuple, I>(first_field_ref)));
      });

      return utils::StrCombine("INSERT INTO \"",
                               table_name,
                               "\" ( ",
                               column_names_joined,
                               " ) VALUES( ",
                               utils::StrJoin(", ", column_values),
                               " );");
    };
    return f;
  }

  std::unique_ptr<TableInfo> tmp_ = nullptr;
  const TableInfo* kTableInfo_    = nullptr;
  void* first_field_ref_          = nullptr;
};

}  // namespace sqliteol