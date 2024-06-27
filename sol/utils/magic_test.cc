#include "sol/utils/magic.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace magic::testing {

TEST(MagicTest, ForRange) {
  std::tuple<int, double, std::string> test_tuple{1, 3.14, "hello"};

  ForRange<0, 3>([&]<int I>() {
    static_assert(I < 3);
    static_assert(I >= 0);

    using cur_type = std::tuple_element_t<I, decltype(test_tuple)>;
    auto cur_value = std::get<I>(test_tuple);

    if constexpr (I == 0) {
      EXPECT_EQ(cur_value, 1);
      EXPECT_EQ(typeid(cur_type), typeid(int));
    } else if constexpr (I == 1) {
      EXPECT_DOUBLE_EQ(cur_value, 3.14);
      EXPECT_EQ(typeid(cur_type), typeid(double));
    } else if constexpr (I == 2) {
      EXPECT_EQ(cur_value, "hello");
      EXPECT_EQ(typeid(cur_type), typeid(std::string));
    }
  });
}

}  // namespace magic::testing

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
