
#pragma once

#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace sqliteol {
namespace utils {

/*
 * use this function to join strings with a separator
 * @example StrJoin(", ", {"a", "b", "c"}) -> "a, b, c"
 */
template <std::ranges::input_range Range>
std::string StrJoin(std::string_view separator, const Range& range) {
  std::ostringstream result;
  auto iter      = std::ranges::begin(range);
  const auto end = std::ranges::end(range);

  if (iter != end) {
    result << *iter++;
  }

  while (iter != end) {
    result << separator << *iter++;
  }

  return result.str();
}

/*
 * Use this function to combine multiple strings into a single string.
 * @example StrCombine("Hello", " ", "World") -> "Hello World"
 */
template <typename... Args>
std::string StrCombine(Args&&... args) {
  static_assert((... && std::is_convertible_v<Args, std::string_view>),
                "All arguments must be convertible to std::string_view.");
  auto StrCombineImpl = [](auto&&... views) {
    std::string result;
    result.reserve((0 + ... + views.size()));
    ((result += views), ...);
    return result;
  };
  return StrCombineImpl(std::string_view(args)...);
}

}  // namespace utils
}  // namespace sqliteol