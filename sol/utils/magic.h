#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

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

template <typename T, std::size_t Offset, std::size_t Align>
constexpr std::size_t AlignedOffset() {
  return (Offset + Align - 1) & ~(Align - 1);
}

// compute the offset of the I-th element in a tuple, needs to consider the alignment
template <typename Tuple, std::size_t I>
constexpr std::size_t GetTupleValueOffset() {
  if constexpr (I == 0) {
    return 0;
  } else {
    constexpr std::size_t prev_offset   = GetTupleValueOffset<Tuple, I - 1>();
    constexpr std::size_t prev_size     = sizeof(std::tuple_element_t<I - 1, Tuple>);
    constexpr std::size_t current_align = alignof(std::tuple_element_t<I, Tuple>);

    return AlignedOffset<std::tuple_element_t<I, Tuple>,
                         prev_offset + prev_size,
                         current_align>();
  }
}

template <typename Tuple, std::size_t I>
std::tuple_element_t<I, Tuple>* GetFieldRef(void* first_field_ref) {
  if constexpr (I == 0) {
    return reinterpret_cast<std::tuple_element_t<I, Tuple>*>(first_field_ref);
  } else {
    return reinterpret_cast<std::tuple_element_t<I, Tuple>*>(
        reinterpret_cast<char*>(first_field_ref) + GetTupleValueOffset<Tuple, I>());
  }
}

}  // namespace magic