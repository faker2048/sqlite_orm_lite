#include "sol/serialize_template.h"

#include <limits>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace sqliteol;
using namespace testing;

TEST(SerializeBaseTest, IntSerialization) {
  int test_value             = 42;
  auto string_representation = ToDataBaseString(test_value);
  EXPECT_THAT(string_representation, Eq("42"));

  int restored_value = FromDataBaseString<int>(string_representation);
  EXPECT_THAT(restored_value, Eq(test_value));
}

TEST(SerializeBaseTest, FloatSerialization) {
  double test_value          = 3.14159;
  auto string_representation = ToDataBaseString(test_value);
  EXPECT_THAT(string_representation, StrEq("3.141590"));

  double restored_value = FromDataBaseString<double>(string_representation);
  EXPECT_NEAR(restored_value, test_value, 1e-6);
}

TEST(SerializeBaseTest, StringSerialization) {
  std::string test_value     = "Hello, world!";
  auto string_representation = ToDataBaseString(test_value);
  EXPECT_THAT(string_representation, Eq("Hello, world!"));

  std::string restored_value = FromDataBaseString<std::string>(test_value);
  EXPECT_THAT(restored_value, Eq(test_value));
}

// CUSTOM TYPE SERIALIZATION

struct MyCustomType {
  int id;
  std::string name;
};

namespace sqliteol {
template <>
constexpr std::string_view ToDataBaseType<MyCustomType>() {
  return "TEXT";
}

template <>
std::string ToDataBaseString(const MyCustomType& value) {
  return std::to_string(value.id) + "|" + value.name;
}

template <>
MyCustomType FromDataBaseString<MyCustomType>(std::string_view str) {
  auto pos = str.find('|');
  return MyCustomType{std::stoi(std::string(str.substr(0, pos))),
                      std::string(str.substr(pos + 1))};
}
}  // namespace sqliteol

TEST(SerializeBaseTest, CustomTypeSerialization) {
  MyCustomType test_value{42, "TestName"};
  auto string_representation = ToDataBaseString(test_value);
  EXPECT_THAT(string_representation, Eq("42|TestName"));

  MyCustomType restored_value = FromDataBaseString<MyCustomType>(string_representation);
  EXPECT_THAT(restored_value.id, Eq(test_value.id));
  EXPECT_THAT(restored_value.name, Eq(test_value.name));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
