#include "catch2/catch.hpp"
#include "goomrand.h"
#include "mathutils.h"

#include <cstdint>
#include <format>
#include <fstream>
#include <limits>
#include <string>
#include <tuple>
#include <vector>

using namespace GOOM::UTILS;

TEST_CASE("save/restore random state", "[saveRestoreRandState]")
{
  const uint64_t seed = 1000;

  SetRandSeed(seed);
  REQUIRE(seed == GetRandSeed());

  const uint32_t r1 = GetRand();
  const uint32_t r2 = GetRand();
  REQUIRE(r1 != r2);

  const std::string saveFile = "/tmp/rand.txt";
  std::ofstream fout{saveFile, std::ofstream::out};
  SaveRandState(fout);
  fout.close();
  const uint32_t r_justAfterSave = GetRand();

  // Scramble things a bit
  SetRandSeed(seed + 10);
  uint32_t r = 0;
  for (size_t i = 0; i < 1000; i++)
  {
    r = GetRand();
  }
  REQUIRE(seed != GetRandSeed());
  REQUIRE(r != r_justAfterSave);

  std::ifstream fin{saveFile, std::ifstream::in};
  RestoreRandState(fin);
  r = GetRand();
  REQUIRE(seed == GetRandSeed());
  REQUIRE(r == r_justAfterSave);
}

TEST_CASE("repeatable random sequence", "[repeatableRandomSequence]")
{
  const uint64_t seed = 1000;

  SetRandSeed(seed);
  REQUIRE(seed == GetRandSeed());
  std::vector<uint32_t> seq1(1000);
  std::vector<float> fseq1(1000);
  for (size_t i = 0; i < 1000; i++)
  {
    seq1[i] = GetRand();
    fseq1[i] = GetRandInRange(0.0F, 1.0F);
  }

  SetRandSeed(seed);
  REQUIRE(seed == GetRandSeed());
  std::vector<uint32_t> seq2(1000);
  std::vector<float> fseq2(1000);
  for (size_t i = 0; i < 1000; i++)
  {
    seq2[i] = GetRand();
    fseq2[i] = GetRandInRange(0.0F, 1.0F);
  }

  SetRandSeed(seed + 1);
  REQUIRE(seed + 1 == GetRandSeed());
  std::vector<uint32_t> seq3(1000);
  std::vector<float> fseq3(1000);
  for (size_t i = 0; i < 1000; i++)
  {
    seq3[i] = GetRand();
    fseq3[i] = GetRandInRange(0.0F, 1.0F);
  }

  REQUIRE(seq1 == seq2);
  REQUIRE(seq1 != seq3);

  REQUIRE(fseq1 == fseq2);
  REQUIRE(fseq1 != fseq3);
}

template<typename valtype>
auto GetMinMax(const size_t numLoop, const valtype& nMin, const valtype& nMax)
    -> std::tuple<valtype, valtype>
{
  valtype min = std::numeric_limits<valtype>::max();
  valtype max = std::numeric_limits<valtype>::min();
  for (size_t i = 0; i < numLoop; i++)
  {
    valtype r = GetRandInRange(nMin, nMax);
    if (r < min)
    {
      min = r;
    }
    if (r > max)
    {
      max = r;
    }
  }

  return std::make_tuple(min, max);
}

TEST_CASE("uint32_t min max get random", "[uintMinMaxGetRandom]")
{
  // After a big enough loop, a good random distribution should have
  // covered the entire range: nMin <= n < nMax
  constexpr size_t numLoop = 100000;

  constexpr uint32_t nMin1 = 999;
  constexpr uint32_t nMax1 = 10001;
  const auto [min1, max1] = GetMinMax(numLoop, nMin1, nMax1);
  REQUIRE(min1 == nMin1);
  REQUIRE(max1 == nMax1 - 1);

  constexpr uint32_t nMin2 = 0;
  constexpr uint32_t nMax2 = 120;
  const auto [min2, max2] = GetMinMax(numLoop, nMin2, nMax2);
  REQUIRE(min2 == nMin2);
  REQUIRE(max2 == nMax2 - 1);

  REQUIRE_NOTHROW(GetRandInRange(5U, 6U));
  REQUIRE_THROWS_WITH(GetRandInRange(5U, 1U), "uint n0 >= n1");
}

TEST_CASE("int32_t min max get random", "[intMinMaxGetRandom]")
{
  // After a big enough loop, a good random distribution should have
  // covered the entire range: nMin <= n < nMax
  constexpr size_t numLoop = 100000;

  constexpr int32_t nMin1 = -999;
  constexpr int32_t nMax1 = 10001;
  const auto [min1, max1] = GetMinMax(numLoop, nMin1, nMax1);
  REQUIRE(min1 == nMin1);
  REQUIRE(max1 == nMax1 - 1);

  constexpr int32_t nMin2 = -999;
  constexpr int32_t nMax2 = -50;
  const auto [min2, max2] = GetMinMax(numLoop, nMin2, nMax2);
  REQUIRE(min2 == nMin2);
  REQUIRE(max2 == nMax2 - 1);

  constexpr int32_t nMin3 = 1;
  constexpr int32_t nMax3 = 999;
  const auto [min3, max3] = GetMinMax(numLoop, nMin3, nMax3);
  REQUIRE(min3 == nMin3);
  REQUIRE(max3 == nMax3 - 1);

  constexpr int32_t nMin4 = 0;
  constexpr int32_t nMax4 = 635;
  const auto [min4, max4] = GetMinMax(numLoop, nMin4, nMax4);
  REQUIRE(min4 == nMin4);
  REQUIRE(max4 == nMax4 - 1);

  REQUIRE_NOTHROW(GetRandInRange(5, 6));
  REQUIRE_NOTHROW(GetRandInRange(-6, -5));
  REQUIRE_NOTHROW(GetRandInRange(-6, 10));
  REQUIRE_THROWS_WITH(GetRandInRange(-5, -6), "int n0 >= n1");
  REQUIRE_THROWS_WITH(GetRandInRange(5, 1), "int n0 >= n1");
  REQUIRE_THROWS_WITH(GetRandInRange(5, -1), "int n0 >= n1");
}

TEST_CASE("float min max get random", "[fltMinMaxGetRandom]")
{
  // After a big enough loop, a good random distribution should have
  // covered the entire range: nMin <= n < nMax
  constexpr size_t numLoop = 1000000;

  constexpr float nMin1 = 0;
  constexpr float nMax1 = 1;
  const auto [min1, max1] = GetMinMax(numLoop, nMin1, nMax1);
  REQUIRE(std::fabs(min1 - nMin1) < 0.0001);
  REQUIRE(std::fabs(max1 - nMax1) < 0.0001);

  constexpr float nMin2 = -1;
  constexpr float nMax2 = 0;
  const auto [min2, max2] = GetMinMax(numLoop, nMin2, nMax2);
  REQUIRE(std::fabs(min2 - nMin2) < 0.0001);
  REQUIRE(std::fabs(max2 - nMax2) < 0.0001);

  constexpr float nMin3 = -10;
  constexpr float nMax3 = +10;
  const auto [min3, max3] = GetMinMax(numLoop, nMin3, nMax3);
  REQUIRE(std::fabs(min3 - nMin3) < 0.0001);
  REQUIRE(std::fabs(max3 - nMax3) < 0.0001);

  REQUIRE_NOTHROW(GetRandInRange(5.0F, 6.0F));
  REQUIRE_NOTHROW(GetRandInRange(-6.0F, -5.0F));
  REQUIRE_NOTHROW(GetRandInRange(-6.0F, 10.0F));
  REQUIRE_THROWS_WITH(GetRandInRange(-5.0F, -6.0F), "float x0 >= x1");
  REQUIRE_THROWS_WITH(GetRandInRange(5.0F, 1.0F), "float x0 >= x1");
  REQUIRE_THROWS_WITH(GetRandInRange(5.0F, -1.0F), "float x0 >= x1");
}

TEST_CASE("weighted events", "[weightedEvents]")
{
  enum class Events
  {
    EVENT1,
    EVENT2,
    EVENT3,
    EVENT4,
    _SIZE
  };
  const Weights<Events> WEIGHTED_EVENTS{{
      {Events::EVENT1, 5},
      {Events::EVENT2, 2},
      {Events::EVENT3, 10},
      {Events::EVENT4, 6},
  }};
  const size_t NUM_EVENTS = static_cast<size_t>(Events::_SIZE);

  constexpr size_t numLoop = 10000000;
  std::array<uint32_t, NUM_EVENTS> counts{};
  for (size_t i = 0; i < numLoop; ++i)
  {
    const auto event = WEIGHTED_EVENTS.GetRandomWeighted();
    counts[static_cast<size_t>(event)]++;
  }

  const size_t sumOfWeights = WEIGHTED_EVENTS.GetSumOfWeights();
  for (size_t i = 0; i < NUM_EVENTS; ++i)
  {
    const double countFrac = static_cast<double>(counts[i]) / static_cast<double>(numLoop);
    const double eventFrac =
        static_cast<double>(WEIGHTED_EVENTS.GetWeight(static_cast<Events>(i))) /
        static_cast<double>(sumOfWeights);
    REQUIRE(floats_equal(countFrac, eventFrac, 0.001F));
    UNSCOPED_INFO(std20::format("i = {}, countFrac = {}, eventFrac = {}", i, countFrac, eventFrac));
  }
}
