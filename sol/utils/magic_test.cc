#include "sol/utils/magic.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace magic::testing {

TEST(MagicTest, ForRangeFunc) {
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

TEST(MagicTest, GetTupleValueOffsetFunc) {
  using TestTuple = std::tuple<int, double, std::string>;
  struct TestStruct {
    int a;
    double b;
    std::string c;
  };
  TestStruct test_struct{1, 3.14, "hello"};
  void* first_field_ref = &test_struct;

  constexpr std::size_t offset_0 = GetTupleValueOffset<TestTuple, 0>();
  constexpr std::size_t offset_1 = GetTupleValueOffset<TestTuple, 1>();
  constexpr std::size_t offset_2 = GetTupleValueOffset<TestTuple, 2>();

  EXPECT_EQ(&test_struct.a, (int*)(first_field_ref));
  EXPECT_EQ((char*)&test_struct.b, (char*)first_field_ref + offset_1);
  EXPECT_EQ((char*)&test_struct.c, (char*)first_field_ref + offset_2);
}

TEST(MagicTest, GetAlignedRefByIndexFunc) {
  using TestTuple = std::tuple<int, double, std::string>;
  struct TestStruct {
    int a;
    double b;
    std::string c;
  };
  TestStruct test_struct{1, 3.14, "hello"};
  void* first_field_ref = &test_struct;

  auto* field_ref_0 = GetAlignedRefByIndex<TestTuple, 0>(first_field_ref);
  auto* field_ref_1 = GetAlignedRefByIndex<TestTuple, 1>(first_field_ref);
  auto* field_ref_2 = GetAlignedRefByIndex<TestTuple, 2>(first_field_ref);
  static_assert(std::is_same_v<decltype(field_ref_0), int*>);
  static_assert(std::is_same_v<decltype(field_ref_1), double*>);
  static_assert(std::is_same_v<decltype(field_ref_2), std::string*>);

  EXPECT_EQ(*field_ref_0, 1);
  EXPECT_DOUBLE_EQ(*field_ref_1, 3.14);
  EXPECT_EQ(*field_ref_2, "hello");
}

TEST(MagicTest, GetAlignedRefByIndexFuncForceTest) {
  using TestTuple =
      std::tuple<int, double, std::string, float, int64_t, int32_t, int8_t, char>;
  struct TestStruct {
    int a;
    double b;
    std::string c;
    float d;
    int64_t e;
    int32_t f;
    int8_t g;
    char h;
  };

  TestStruct test_struct{1, 3.14, "hello", 3.14f, 1234567890, 1234567890, 123, 'a'};
  TestTuple test_struct_tuple =
      std::make_tuple(1, 3.14, "hello", 3.14f, 1234567890, 1234567890, 123, 'a');
  void* first_field_ref = &test_struct;

  constexpr std::size_t tuple_size = std::tuple_size_v<TestTuple>;

  magic::ForRange<0, tuple_size>([&]<int I>() {
    auto* field_ref = magic::GetAlignedRefByIndex<TestTuple, I>(first_field_ref);
    auto& value     = std::get<I>(test_struct_tuple);
    EXPECT_EQ(*field_ref, value);
  });
}

}  // namespace magic::testing

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
