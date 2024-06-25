#pragma once

#include <type_traits>

namespace magic {

/**
 * Executes a compile-time loop from `begin` to `end`, invoking a callable object
 * for each iteration. This template uses constexpr if and template recursion
 * to unroll loops at compile time.
 *
 * Example usage:
 *   std::tuple<int, double, char> my_tuple{1, 2.0, '3'};
 *   ForRange<0, 3>([&]<int I>() {
 *     using cur_type = std::tuple_element_t<I, decltype(my_tuple)>;
 *     auto cur_value = std::get<I>(my_tuple);
 *     std::cout << "value: " << cur_value << ", type: " << typeid(cur_type).name() <<
 * std::endl;
 *   });
 *
 * @tparam begin The starting index of the loop (inclusive).
 * @tparam end The ending index of the loop (exclusive).
 * @tparam Func The type of the callable object.
 * @param f The callable object to be invoked for each iteration.
 *         The callable must be invocable with a template parameter specifying the current
 * index.
 */
template <int begin, int end, typename Func>
void ForRange(Func&& f) {
  if constexpr (begin < end) {
    f.template operator()<begin>();
    ForRange<begin + 1, end>(std::forward<Func>(f));
  }
}

/**
 * A helper struct that always evaluates to false. This is useful for static_asserts
 * that should always fail.
 *
 * Example usage:
 *   static_assert(AlwaysFalse<int>, "This static_assert will always fail");
 */
template <typename T>
struct AlwaysFalse : std::false_type {};

}  // namespace magic