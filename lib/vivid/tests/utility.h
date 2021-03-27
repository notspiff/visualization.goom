#pragma once

#include "vivid/types.h"

#include <random>


inline float randf(const float from = 0.f, const float to = 1.f)
{
  static std::random_device rd;
  static std::mt19937 mt(rd());

  std::uniform_real_distribution<> dist(from, to);
  return dist(mt);
}


inline int randi(const int from = 0, const int to = 255)
{
  static std::random_device rd;
  static std::mt19937 mt(rd());

  std::uniform_int_distribution<> dist(from, to);
  return dist(mt);
}


#if __cplusplus <= 201402L
namespace vivid
{
namespace rgb
{
#else
namespace vivid::rgb
{
#endif
inline rgb_t rand()
{
  return {randf(), randf(), randf()};
}
#if __cplusplus <= 201402L
} // namespace rgb
} // namespace vivid
#else
} // namespace vivid::rgb
#endif


#if __cplusplus <= 201402L
namespace vivid
{
namespace rgb8
{
#else
namespace vivid::rgb8
{
#endif
inline col8_t rand()
{
  return {randi(), randi(), randi()};
}
#if __cplusplus <= 201402L
} // namespace rgb8
} // namespace vivid
#else
} // namespace vivid::rgb
#endif
