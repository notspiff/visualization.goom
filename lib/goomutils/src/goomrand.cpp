#include "goomutils/goomrand.h"

#include "goomutils/logging_control.h"
//#undef NO_LOGGING
#include "goomutils/logging.h"
#if __cplusplus <= 201402L
#include "goomutils/mathutils.h"
#endif
#include "goomutils/splitmix.hpp"
#include "goomutils/xoshiro.hpp"

#undef NDEBUG

#include <cmath>
#include <cstdint>
#include <limits>
#include <ostream>
#include <random>
#include <stdexcept>

#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

const uint32_t g_randMax = (xoshiro256plus64::max() > std::numeric_limits<uint32_t>::max())
                               ? std::numeric_limits<uint32_t>::max()
                               : xoshiro256plus64::max();

// NOTE: C++ std::uniform_int_distribution is too expensive (about double) so we use Xoshiro
//   and multiplication/shift technique. For timings, see tests/test_goomrand.cpp.
// thread_local xoshiro256starstar64 eng { GetRandSeed() };

thread_local uint64_t randSeed = std::random_device{}();
thread_local xoshiro256plus64 xoshiroEng{GetRandSeed()};

auto GetRandSeed() -> uint64_t
{
  return randSeed;
}

void SetRandSeed(uint64_t seed)
{
  randSeed = seed;
  xoshiroEng = GetRandSeed();
}

inline auto RandXoshiroFunc(const uint32_t n0, const uint32_t n1) -> uint32_t
{
  const uint32_t x = xoshiroEng();
  const uint64_t m = (static_cast<uint64_t>(x) * static_cast<uint64_t>(n1 - n0)) >> 32;
  return n0 + static_cast<uint32_t>(m);
}

inline auto RandSplitmixFunc(const uint32_t n0, const uint32_t n1) -> uint32_t
{
  // thread_local splitmix32 eng { GetRandSeed() };
  thread_local splitmix64 s_eng{GetRandSeed()};
  const uint32_t x = s_eng();
  const uint64_t m = (static_cast<uint64_t>(x) * static_cast<uint64_t>(n1 - n0)) >> 32;
  return n0 + static_cast<uint32_t>(m);
}

void SaveRandState(std::ostream& f)
{
  f << randSeed << std::endl;
  f << xoshiroEng << std::endl;
}

void RestoreRandState(std::istream& f)
{
  f >> randSeed;
  f >> xoshiroEng;
}

auto GetRandInRange(const uint32_t n0, const uint32_t n1) -> uint32_t
{
#ifndef NDEBUG
  if (n0 >= n1)
  {
    throw std::logic_error("uint n0 >= n1");
  }
#endif
#ifndef NO_LOGGING
  const uint32_t randVal = randXoshiroFunc(n0, n1);
  logDebug("randVal = {}", randVal);
  return randVal;
#endif

  return RandXoshiroFunc(n0, n1);
}

auto GetRandInRange(const int32_t n0, const int32_t n1) -> int32_t
{
  if (n0 >= n1)
  {
    throw std::logic_error("int n0 >= n1");
  }
  if ((n0 >= 0) && (n1 >= 0))
  {
    return static_cast<int32_t>(
        GetRandInRange(static_cast<uint32_t>(n0), static_cast<uint32_t>(n1)));
  }
  return n0 + static_cast<int32_t>(GetNRand(static_cast<uint32_t>(-n0 + n1)));
}

auto GetRandInRange(const float x0, const float x1) -> float
{
#ifndef NDEBUG
  if (x0 >= x1)
  {
    throw std::logic_error("float x0 >= x1");
  }
#endif

  static const auto s_eng_max = static_cast<float>(g_randMax);
  const float t = static_cast<float>(RandXoshiroFunc(0, g_randMax)) / s_eng_max;
  return stdnew::lerp(x0, x1, t);
  //  thread_local std::uniform_real_distribution<> dis(0, 1);
  //  return std::lerp(x0, x1, static_cast<float>(dis(eng)));
}

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
