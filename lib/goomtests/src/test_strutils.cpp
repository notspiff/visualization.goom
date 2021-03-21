#include "catch2/catch.hpp"
#include "strutils.h"

#include <string>
#include <vector>

using namespace GOOM::UTILS;

TEST_CASE("SplitString", "[SplitString]")
{
  const std::string testString1 = "line1: word1, word2\nline2: word3, word4\n";

  const std::vector<std::string> test1 = SplitString(testString1, ",");
  UNSCOPED_INFO("testString1 = \"" << testString1 + "\"");
  for (const auto& s : test1)
  {
    UNSCOPED_INFO("s = " << s);
  }
  REQUIRE(test1.size() == 3);
  REQUIRE(test1[0] == "line1: word1");
  REQUIRE(test1[1] == " word2\nline2: word3");
  REQUIRE(test1[2] == " word4\n");

  const std::vector<std::string> test2 = SplitString(testString1, "\n");
  REQUIRE(test2.size() == 2);
  REQUIRE(test2[0] == "line1: word1, word2");
  REQUIRE(test2[1] == "line2: word3, word4");

  const std::string testString2 = "word1; word2; word3; word4";
  const std::vector<std::string> test3 = SplitString(testString2, "; ");
  REQUIRE(test3.size() == 4);
  REQUIRE(test3[0] == "word1");
  REQUIRE(test3[1] == "word2");
  REQUIRE(test3[2] == "word3");
  REQUIRE(test3[3] == "word4");

  const std::string testString3 = "word1 \nword2\nword3 \nword4 ";
  const std::vector<std::string> test4 = SplitString(testString3, "\n");
  REQUIRE(test4.size() == 4);
  REQUIRE(test4[0] == "word1 ");
  REQUIRE(test4[1] == "word2");
  REQUIRE(test4[2] == "word3 ");
  REQUIRE(test4[3] == "word4 ");
}

TEST_CASE("EnumToString", "[EnumToString]")
{
  enum class EnumTester
  {
    _NULL = -1,
    TEST1,
    TEST2,
    TEST3,
    _SIZE
  };

  EnumTester test = EnumTester::_NULL;
  REQUIRE(EnumToString(test) == "_NULL");
  test = EnumTester::TEST1;
  REQUIRE(EnumToString(test) == "TEST1");
  test = EnumTester::TEST2;
  REQUIRE(EnumToString(test) == "TEST2");
  test = EnumTester::_SIZE;
  REQUIRE(EnumToString(test) == "_SIZE");

  REQUIRE(EnumToString(EnumTester::TEST3) == "TEST3");
}
