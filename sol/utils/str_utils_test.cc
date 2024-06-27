#include "sol/utils/str_utils.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace sqliteol {
namespace utils {
namespace testing {

class StrUtilsTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Initialization code here
  }

  void TearDown() override {
    // Clean-up code here
  }
};

// Test cases for StrJoin
TEST_F(StrUtilsTest, JoinsMultipleStrings) {
  std::vector<std::string> v = {"a", "b", "c"};
  EXPECT_EQ(StrJoin(", ", v), "a, b, c");
}

TEST_F(StrUtilsTest, JoinsSingleString) {
  std::vector<std::string> v = {"a"};
  EXPECT_EQ(StrJoin(", ", v), "a");
}

TEST_F(StrUtilsTest, JoinsNoStrings) {
  std::vector<std::string> v = {};
  EXPECT_EQ(StrJoin(", ", v), "");
}

TEST_F(StrUtilsTest, JoinsWithEmptySeparator) {
  std::vector<std::string> v = {"a", "b", "c"};
  EXPECT_EQ(StrJoin("", v), "abc");
}

// Test cases for StrCombine
TEST_F(StrUtilsTest, CombinesMultipleStrings) {
  EXPECT_EQ(StrCombine("Hello", " ", "World"), "Hello World");
}

TEST_F(StrUtilsTest, CombinesStringsAndEmpty) {
  EXPECT_EQ(StrCombine("Hello", "", "World"), "HelloWorld");
}

TEST_F(StrUtilsTest, CombinesSingleString) {
  EXPECT_EQ(StrCombine("Hello"), "Hello");
}

TEST_F(StrUtilsTest, CombinesNoStrings) {
  EXPECT_EQ(StrCombine(), "");
}

}  // namespace testing
}  // namespace utils
}  // namespace sqliteol

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
