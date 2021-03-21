#include "catch2/catch.hpp"
#include "mathutils.h"

#include <cmath>

using namespace GOOM;
using namespace UTILS;

/*****
TEST_CASE("Test MathApprox", "[MathApprox]")
{
  constexpr float SMALL_DIFF = 0.001F;

  const float step = m_two_pi / static_cast<float>(MathApprox::NUM_SIN_COS_ANGLES);
  float angle = -static_cast<int>(MathApprox::NUM_SIN_COS_ANGLES) * step;
  const size_t N = 2 * MathApprox::NUM_SIN_COS_ANGLES;
  for (size_t i = 0; i < N; ++i)
  {
    UNSCOPED_INFO("i = " << i);
    UNSCOPED_INFO("angle = " << angle);
    UNSCOPED_INFO("ToDegrees(angle) = " << MathApprox::ToDegrees(angle));
    UNSCOPED_INFO("MathApprox::sin(angle) = " << MathApprox::fsin(angle));
    UNSCOPED_INFO("std::sin(angle) = " << std::sin(angle));
    REQUIRE(floats_equal(std::sin(angle), MathApprox::fsin(angle), SMALL_DIFF));

    UNSCOPED_INFO("i = " << i);
    UNSCOPED_INFO("angle = " << angle);
    UNSCOPED_INFO("angle/step = " << (angle + m_two_pi) / step);
    UNSCOPED_INFO("ToDegrees(angle) = " << MathApprox::ToDegrees(angle));
    UNSCOPED_INFO("MathApprox::cos(angle) = " << MathApprox::fcos(angle));
    UNSCOPED_INFO("std::cos(angle) = " << std::cos(angle));
    REQUIRE(floats_equal(std::cos(angle), MathApprox::fcos(angle), SMALL_DIFF));

    angle += step;
  }
}
****/
