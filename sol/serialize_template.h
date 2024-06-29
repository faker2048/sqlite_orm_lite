#pragma once

#include <string>
#include <string_view>
#include <type_traits>

#include "sol/utils/magic.h"
#include "sol/utils/str_utils.h"

/**
 * @file
 * Provides functionality for type conversions between C++ types and database strings.
 *
 * @details This header defines templates that convert C++ data types to and from strings
 *          that can be stored in a database. It uses specializations for common types
 *          like integral types, floating-point types, and strings.
 */

namespace sqliteol {
/**
 * @concept Convertible
 * @brief Checks for type convertibility to SQLite-compatible strings.
 *
 * @tparam T The type to check for convertibility.
 */
template <typename T>
concept Convertible = requires(T a) {
  { ToDataBaseType<T>() } -> std::convertible_to<std::string_view>;
  { ToDataBaseString(a) } -> std::convertible_to<std::string>;
  { FromDataBaseString<T>(std::string{}) } -> std::convertible_to<T>;
};

template <typename T>
constexpr std::string_view ToDataBaseType() {
  static_assert(Convertible<T>, "Unsupported type for ToDataBaseType");
  return "";  // Default, should never be hit if static_assert works
}

template <typename T>
std::string ToDataBaseString(const T& value) {
  static_assert(Convertible<T>, "Unsupported type for ToDataBaseString");
  return "";  // Default, should never be hit if static_assert works
}

template <typename T>
T FromDataBaseString(std::string_view str) {
  static_assert(Convertible<T>, "Unsupported type for FromDataBaseString");
  return T{};  // Default, should never be hit if static_assert works
}

}  // namespace sqliteol

/**********************************
 * C++ BASE TYPES SPECIALIZATIONS *
 **********************************/
namespace sqliteol {

// Integral types specializations
template <std::integral T>
constexpr std::string_view ToDataBaseType() {
  return "INT";
}

template <std::integral T>
std::string ToDataBaseString(const T& value) {
  return std::to_string(value);
}

template <std::integral T>
T FromDataBaseString(std::string_view str) {
  return static_cast<T>(std::stoll(std::string(str)));
}

// Floating-point types specializations
template <std::floating_point T>
constexpr std::string_view ToDataBaseType() {
  return "REAL";
}

template <std::floating_point T>
std::string ToDataBaseString(const T& value) {
  return std::to_string(value);
}

template <std::floating_point T>
T FromDataBaseString(std::string_view str) {
  return static_cast<T>(std::stold(std::string(str)));
}

// String type specialization
template <>
constexpr std::string_view ToDataBaseType<std::string>() {
  return "TEXT";
}

template <>
std::string ToDataBaseString(const std::string& value) {
  return utils::StrCombine("'", value, "'");
}

template <>
std::string FromDataBaseString<std::string>(std::string_view str) {
  return std::string(str);
}

}  // namespace sqliteol

/**********************************
 * EXAMPLE USAGE FOR CUSTOM TYPES *
 **********************************/

/*
 * Example usage for a custom type:
 * struct MyCustomType {
 *   int id;
 *   std::string name;
 * };
 *
 * template <>
 * constexpr std::string_view sqliteol::ToDataBaseType<MyCustomType>() {
 *   return "TEXT";
 * }
 *
 * template <>
 * std::string sqliteol::ToDataBaseString(const MyCustomType& value) {
 *   return std::to_string(value.id) + "|" + value.name;
 * }
 *
 * template <>
 * MyCustomType sqliteol::FromDataBaseString<MyCustomType>(std::string_view str) {
 *   auto pos = str.find('|');
 *   return MyCustomType{std::stoi(std::string(str.substr(0, pos))),
 *                       std::string(str.substr(pos + 1))};
 * }
 *
 * int main() {
 *   MyCustomType my_custom_type{42, "hello"};
 *   std::string db_string = sqliteol::ToDataBaseString(my_custom_type); // "42|hello"
 *   MyCustomType my_custom_type_copy =
 *   sqliteol::FromDataBaseString<MyCustomType>(db_string); // {42, "hello"}
 *   return 0;
 * }
 */
