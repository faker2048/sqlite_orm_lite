#pragma once

#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "sol/serialize_template.h"
#include "sol/sql_constructor.h"
#include "sol/sql_constructor_build_cache.h"

namespace sqliteol {

template <typename RowTuple>
class SqlConstructor;

template <typename... CurColumnTypes>
class SqlConstructorBuilder {
 public:
  using CurRowTuple = std::tuple<CurColumnTypes...>;
  using BuildCache  = SqlConstructorBuildCache;
  using TableInfo   = BuildCache::TableInfo;

  template <size_t I>
  static constexpr std::string_view SqliteColumnTypeStr_v =
      ToDataBaseType<std::tuple_element_t<I, CurRowTuple>>();

  inline SqlConstructorBuilder() {
  }

  template <typename... ColumnTypes>
  SqlConstructorBuilder(std::unique_ptr<TableInfo>&& tmp_,
                        const TableInfo* kTableInfo,
                        void* first_field_ref)
      : tmp_(std::move(tmp_)),
        kTableInfo_(kTableInfo),
        first_field_ref_(first_field_ref) {
  }

  inline SqlConstructorBuilder<CurColumnTypes...>& SetTableName(
      const std::string& table_name) {
    std::optional<const TableInfo*> cached =
        BuildCache::GetInstance().GetTableInfo(table_name);

    if (cached.has_value()) {
      kTableInfo_ = cached.value();
      if (kTableInfo_ == nullptr) {
        throw std::runtime_error("GetTableInfo return nullptr");
      }
    } else {
      tmp_             = std::make_unique<TableInfo>();
      tmp_->table_name = table_name;
    }
    return *this;
  }

  template <typename ColumnType>
  SqlConstructorBuilder<CurColumnTypes..., ColumnType> AddColumn(
      std::string_view column_name, ColumnType* value) {
    if constexpr (std::tuple_size_v<CurRowTuple> == 0) {
      first_field_ref_ = value;
    }

    if (!is_built()) {
      tmp_->column_names.emplace_back(column_name);
    }
    return SqlConstructorBuilder<CurColumnTypes..., ColumnType>(
        std::move(tmp_), kTableInfo_, first_field_ref_);
  }

  inline SqlConstructor<CurRowTuple> Build() {
    if (!is_built()) {
      kTableInfo_ = CreateTableInfo();
    } else if (*(kTableInfo_->row_tuple_type) != typeid(CurRowTuple)) {
      throw std::runtime_error(
          utils::StrCombine("Table name already exists with different row type.",
                            " existing_row_type: ",
                            kTableInfo_->row_tuple_type->name(),
                            " new_row_type: ",
                            typeid(CurRowTuple).name()));
    }

    return SqlConstructor<CurRowTuple>(kTableInfo_, first_field_ref_);
  }

 private:
  inline bool is_built() const {
    return kTableInfo_ != nullptr;
  }

  inline const TableInfo* CreateTableInfo() {
    tmp_->ensure_table_sql = GetEnsureTableSql<CurRowTuple>();
    tmp_->insert_sql_gen   = GetInsertSQLFunc<CurRowTuple>();
    for (size_t i = 0; i < tmp_->column_names.size(); ++i) {
      tmp_->column_name_to_index.emplace(tmp_->column_names[i], i);
    }
    tmp_->row_tuple_type   = &typeid(CurRowTuple);
    std::string table_name = tmp_->table_name;
    BuildCache::GetInstance().AddTableInfo(std::move(*tmp_));
    return BuildCache::GetInstance().GetTableInfo(table_name).value();
  }

  template <typename RowTuple>
  std::string GetEnsureTableSql() const {
    constexpr size_t column_size = std::tuple_size_v<RowTuple>;

    std::vector<std::string> column_spec = {};
    magic::ForRange<0, column_size>([&]<int I>() {
      using ColumnType = std::tuple_element_t<I, RowTuple>;
      column_spec.push_back(
          utils::StrCombine(tmp_->column_names[I], " ", ToDataBaseType<ColumnType>()));
    });

    return utils::StrCombine("CREATE TABLE IF NOT EXISTS \"",
                             tmp_->table_name,
                             "\"( ",
                             utils::StrJoin(", ", column_spec),
                             " );");
  }

  template <typename RowTuple>
  std::function<std::string(const void* first_field_ref)> GetInsertSQLFunc() const {
    constexpr size_t column_size    = std::tuple_size_v<RowTuple>;
    std::string column_names_joined = utils::StrJoin(", ", tmp_->column_names);

    auto f = [table_name = tmp_->table_name,
              column_names_joined](const void* first_field_ref) {
      std::vector<std::string> column_values = {};
      magic::ForRange<0, column_size>([&]<int I>() {
        using ColumnType  = std::tuple_element_t<I, RowTuple>;
        std::string value = ToDataBaseString(*magic::GetAlignedRefByIndex<RowTuple, I>(
            const_cast<void*>(first_field_ref)));
        if constexpr (std::is_same_v<ColumnType, std::string>) {
          value = utils::StrCombine("'", value, "'");
        }
        column_values.push_back(value);
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