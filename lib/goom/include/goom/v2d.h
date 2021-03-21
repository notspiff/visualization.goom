#ifndef VISUALIZATION_GOOM_V2D_H
#define VISUALIZATION_GOOM_V2D_H

#include <cstdint>

namespace GOOM
{

struct V2dFlt;

struct V2dInt
{
  int32_t x = 0;
  int32_t y = 0;
  [[nodiscard]] auto ToFlt() const -> V2dFlt;
};

struct V2dFlt
{
  float x = 0.0;
  float y = 0.0;
  auto operator+=(const V2dFlt& v) -> V2dFlt&;
  auto operator*=(float a) -> V2dFlt&;
  auto operator*(float a) const -> V2dFlt;
};

inline auto V2dInt::ToFlt() const -> V2dFlt
{
  return {static_cast<float>(x), static_cast<float>(y)};
}

[[nodiscard]] inline auto operator*(float a, const V2dFlt& v) -> V2dFlt;

inline auto V2dFlt::operator+=(const V2dFlt& v) -> V2dFlt&
{
  x += v.x;
  y += v.y;
  return *this;
}

inline auto V2dFlt::operator*=(const float a) -> V2dFlt&
{
  x *= a;
  y *= a;
  return *this;
}

inline auto V2dFlt::operator*(const float a) const -> V2dFlt
{
  return {a * x, a * y};
}

inline auto operator*(const float a, const V2dFlt& v) -> V2dFlt
{
  return v * a;
}

} // namespace GOOM
#endif
