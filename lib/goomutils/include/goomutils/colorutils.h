#ifndef VISUALIZATION_GOOM_LIB_GOOMUTILS_COLORUTILS_H_
#define VISUALIZATION_GOOM_LIB_GOOMUTILS_COLORUTILS_H_

#include "goom/goom_graphic.h"
#include "mathutils.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>

#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

auto GetIntColor(uint8_t r, uint8_t g, uint8_t b) -> Pixel;

auto GetColorAverage(const std::vector<Pixel>& colors) -> Pixel;
auto GetColorAverage(const Pixel& color1, const Pixel& color2) -> Pixel;
auto GetColorBlend(const Pixel& srce, const Pixel& dest) -> Pixel;
auto GetColorMultiply(const Pixel& srce, const Pixel& dest, bool allowOverexposed) -> Pixel;
auto GetColorAdd(const Pixel& color1, const Pixel& color2, bool allowOverexposed) -> Pixel;
auto GetColorSubtract(const Pixel& color1, const Pixel& color2) -> Pixel;
auto GetBrighterColorInt(uint32_t brightness, const Pixel& color, bool allowOverexposed) -> Pixel;
auto GetBrighterColorInt(const float brightness, const Pixel&, const bool) -> Pixel = delete;

auto GetBrighterColor(float brightness, const Pixel& color, bool allowOverexposed) -> Pixel;
auto GetBrighterColor(const uint32_t brightness, const Pixel&, const bool) -> Pixel = delete;

auto GetRightShiftedChannels(const Pixel& color, int value) -> Pixel;
auto GetHalfIntensityColor(const Pixel& color) -> Pixel;

auto GetLightenedColor(const Pixel& oldColor, float power) -> Pixel;
auto GetEvolvedColor(const Pixel& baseColor) -> Pixel;

auto GetRgbColorChannelLerp(int32_t c1, int32_t c2, int32_t intT) -> uint32_t;
auto GetRgbColorLerp(const Pixel& colA, const Pixel& colB, float t) -> Pixel;

auto GetLuma(const Pixel& color) -> uint32_t;

class GammaCorrection
{
public:
  GammaCorrection(float gamma, float threshold);
  [[nodiscard]] auto GetCorrection(float brightness, const Pixel& color) const -> Pixel;

private:
  const float m_gammaReciprocal;
  const float m_threshold;
};


inline auto ColorChannelMultiply(const uint8_t c1, const uint8_t c2) -> uint32_t
{
  return (static_cast<uint32_t>(c1) * static_cast<uint32_t>(c2)) >> 8;
}

inline auto ColorChannelAdd(const uint8_t c1, const uint8_t c2) -> uint32_t
{
  return static_cast<uint32_t>(c1) + static_cast<uint32_t>(c2);
}

inline auto ColorChannelSubtract(const uint8_t c1, const uint8_t c2) -> uint32_t
{
  if (c1 < c2)
  {
    return 0;
  }
  return static_cast<uint32_t>(c1) - static_cast<uint32_t>(c2);
}

inline auto GetColorAverage(const std::vector<Pixel>& colors) -> Pixel
{
  uint32_t newR = 0;
  uint32_t newG = 0;
  uint32_t newB = 0;
  uint32_t newA = 0;

  for (const auto& c : colors)
  {
    newR += static_cast<uint32_t>(c.R());
    newG += static_cast<uint32_t>(c.G());
    newB += static_cast<uint32_t>(c.B());
    newA += static_cast<uint32_t>(c.A());
  }

  return Pixel{{
      /*.r = */ static_cast<uint8_t>(newR / colors.size()),
      /*.g = */ static_cast<uint8_t>(newG / colors.size()),
      /*.b = */ static_cast<uint8_t>(newB / colors.size()),
      /*.a = */ static_cast<uint8_t>(newA / colors.size()),
  }};
}

inline auto GetColorAverage(const Pixel& color1, const Pixel& color2) -> Pixel
{
  const uint32_t newR = ColorChannelAdd(color1.R(), color2.R()) >> 1;
  const uint32_t newG = ColorChannelAdd(color1.G(), color2.G()) >> 1;
  const uint32_t newB = ColorChannelAdd(color1.B(), color2.B()) >> 1;
  const uint32_t newA = ColorChannelAdd(color1.A(), color2.A()) >> 1;

  return Pixel{{
      /*.r = */ static_cast<uint8_t>(newR),
      /*.g = */ static_cast<uint8_t>(newG),
      /*.b = */ static_cast<uint8_t>(newB),
      /*.a = */ static_cast<uint8_t>(newA),
  }};
}

inline auto GetColorBlend(const Pixel& srce, const Pixel& dest) -> Pixel
{
  const auto srceR = static_cast<int>(srce.R());
  const auto srceG = static_cast<int>(srce.G());
  const auto srceB = static_cast<int>(srce.B());
  const auto srceA = static_cast<int>(srce.A());
  const auto destR = static_cast<int>(dest.R());
  const auto destG = static_cast<int>(dest.G());
  const auto destB = static_cast<int>(dest.B());
  const auto destA = static_cast<int>(dest.A());

  const auto newR =
      static_cast<uint32_t>(destR + (srceA * (srceR - destR)) / channel_limits<int32_t>::max());
  const auto newG =
      static_cast<uint32_t>(destG + (srceA * (srceG - destG)) / channel_limits<int32_t>::max());
  const auto newB =
      static_cast<uint32_t>(destB + (srceA * (srceB - destB)) / channel_limits<int32_t>::max());
  const auto newA = std::min(channel_limits<int32_t>::max(), srceA + destA);

  return Pixel{{
      /*.r = */ static_cast<u_int8_t>(newR),
      /*.g = */ static_cast<u_int8_t>(newG),
      /*.b = */ static_cast<u_int8_t>(newB),
      /*.a = */ static_cast<u_int8_t>(newA),
  }};
}

inline auto GetColorMultiply(const Pixel& srce, const Pixel& dest, bool allowOverexposed) -> Pixel
{
  uint32_t newR = ColorChannelMultiply(srce.R(), dest.R());
  uint32_t newG = ColorChannelMultiply(srce.G(), dest.G());
  uint32_t newB = ColorChannelMultiply(srce.B(), dest.B());
  const uint32_t newA = ColorChannelMultiply(srce.A(), dest.A());

  if (!allowOverexposed)
  {
    const uint32_t maxVal = std::max({newR, newG, newB});
    if (maxVal > channel_limits<uint32_t>::max())
    {
      // scale all channels back
      newR = (newR << 8) / maxVal;
      newG = (newG << 8) / maxVal;
      newB = (newB << 8) / maxVal;
    }
  }

  return Pixel{{
      /*.r = */ static_cast<uint8_t>((newR & 0xffffff00) ? 0xff : newR),
      /*.g = */ static_cast<uint8_t>((newG & 0xffffff00) ? 0xff : newG),
      /*.b = */ static_cast<uint8_t>((newB & 0xffffff00) ? 0xff : newB),
      /*.a = */ static_cast<uint8_t>((newA & 0xffffff00) ? 0xff : newA),
  }};
}

inline auto GetColorAdd(const Pixel& color1, const Pixel& color2, bool allowOverexposed) -> Pixel
{
  uint32_t newR = ColorChannelAdd(color1.R(), color2.R());
  uint32_t newG = ColorChannelAdd(color1.G(), color2.G());
  uint32_t newB = ColorChannelAdd(color1.B(), color2.B());
  const uint32_t newA = ColorChannelAdd(color1.A(), color2.A());

  if (!allowOverexposed)
  {
    const uint32_t maxVal = std::max({newR, newG, newB});
    if (maxVal > channel_limits<uint32_t>::max())
    {
      // scale all channels back
      newR = (newR << 8) / maxVal;
      newG = (newG << 8) / maxVal;
      newB = (newB << 8) / maxVal;
    }
  }

  return Pixel{{
      /*.r = */ static_cast<uint8_t>((newR & 0xffffff00) ? 0xff : newR),
      /*.g = */ static_cast<uint8_t>((newG & 0xffffff00) ? 0xff : newG),
      /*.b = */ static_cast<uint8_t>((newB & 0xffffff00) ? 0xff : newB),
      /*.a = */ static_cast<uint8_t>((newA & 0xffffff00) ? 0xff : newA),
  }};
}

inline auto GetColorSubtract(const Pixel& color1, const Pixel& color2) -> Pixel
{
  const uint32_t newR = ColorChannelSubtract(color1.R(), color2.R());
  const uint32_t newG = ColorChannelSubtract(color1.G(), color2.G());
  const uint32_t newB = ColorChannelSubtract(color1.B(), color2.B());
  const uint32_t newA = ColorChannelSubtract(color1.A(), color2.A());

  return Pixel{{
      /*.r = */ static_cast<uint8_t>(newR),
      /*.g = */ static_cast<uint8_t>(newG),
      /*.b = */ static_cast<uint8_t>(newB),
      /*.a = */ static_cast<uint8_t>(newA),
  }};
}


inline auto GetBrighterChannelColor(const uint32_t brightness, const uint8_t channelVal) -> uint32_t
{
  return (brightness * static_cast<uint32_t>(channelVal)) >> 8;
}

inline auto GetBrighterColorInt(uint32_t brightness, const Pixel& color, bool allowOverexposed)
    -> Pixel
{
  uint32_t newR = GetBrighterChannelColor(brightness, color.R());
  uint32_t newG = GetBrighterChannelColor(brightness, color.G());
  uint32_t newB = GetBrighterChannelColor(brightness, color.B());
  const uint32_t newA = GetBrighterChannelColor(brightness, color.A());

  if (!allowOverexposed)
  {
    const uint32_t maxVal = std::max({newR, newG, newB});
    if (maxVal > channel_limits<uint32_t>::max())
    {
      // scale all channels back
      newR = (newR << 8) / maxVal;
      newG = (newG << 8) / maxVal;
      newB = (newB << 8) / maxVal;
    }
  }

  return Pixel{{
      /*.r = */ static_cast<uint8_t>((newR & 0xffffff00) ? 0xff : newR),
      /*.g = */ static_cast<uint8_t>((newG & 0xffffff00) ? 0xff : newG),
      /*.b = */ static_cast<uint8_t>((newB & 0xffffff00) ? 0xff : newB),
      /*.a = */ static_cast<uint8_t>((newA & 0xffffff00) ? 0xff : newA),
  }};
}


inline auto GetBrighterColor(float brightness, const Pixel& color, bool allowOverexposed) -> Pixel
{
  assert(brightness >= 0.0 && brightness <= 2.0);
  const auto br = static_cast<uint32_t>(std::round(brightness * 256 + 0.0001f));
  return GetBrighterColorInt(br, color, allowOverexposed);
}


inline auto GetRightShiftedChannels(const Pixel& color, int value) -> Pixel
{
  Pixel p = color;

  p.SetR(p.R() >> value);
  p.SetG(p.G() >> value);
  p.SetB(p.B() >> value);

  return p;
}

inline auto GetRgbColorChannelLerp(int32_t c1, int32_t c2, int32_t intT) -> uint32_t
{
  constexpr auto MAX_COL_VAL_32 = static_cast<int32_t>(MAX_COLOR_VAL);
  return static_cast<uint32_t>((MAX_COL_VAL_32 * c1 + intT * (c2 - c1)) / MAX_COL_VAL_32);
}

inline auto GetRgbColorLerp(const Pixel& colA, const Pixel& colB, float t) -> Pixel
{
  t = stdnew::clamp(t, 0.0F, 1.0F);
  const auto intT = static_cast<int32_t>(t * static_cast<float>(MAX_COLOR_VAL));

  const auto colA_R = static_cast<int32_t>(colA.R());
  const auto colA_G = static_cast<int32_t>(colA.G());
  const auto colA_B = static_cast<int32_t>(colA.B());
  const auto colA_A = static_cast<int32_t>(colA.A());
  const auto colB_R = static_cast<int32_t>(colB.R());
  const auto colB_G = static_cast<int32_t>(colB.G());
  const auto colB_B = static_cast<int32_t>(colB.B());
  const auto colB_A = static_cast<int32_t>(colB.A());

  const uint32_t newR = GetRgbColorChannelLerp(colA_R, colB_R, intT);
  const uint32_t newG = GetRgbColorChannelLerp(colA_G, colB_G, intT);
  const uint32_t newB = GetRgbColorChannelLerp(colA_B, colB_B, intT);
  const uint32_t newA = GetRgbColorChannelLerp(colA_A, colB_A, intT);

  return Pixel{{
      /*.r = */ static_cast<u_int8_t>(newR),
      /*.g = */ static_cast<u_int8_t>(newG),
      /*.b = */ static_cast<u_int8_t>(newB),
      /*.a = */ static_cast<u_int8_t>(newA),
  }};
}


inline auto GetHalfIntensityColor(const Pixel& color) -> Pixel
{
  return GetRightShiftedChannels(color, 1);
}


// RGB -> Luma conversion formula.
//
// Photometric/digital ITU BT.709:
//
//     Y = 0.2126 R + 0.7152 G + 0.0722 B
//
// Digital ITU BT.601 (gives more weight to the R and B components):
//
//     Y = 0.299 R + 0.587 G + 0.114 B
//
// If you are willing to trade accuracy for perfomance, there are two approximation formulas
// for this one:
//
//     Y = 0.33 R + 0.5 G + 0.16 B
//
//     Y = 0.375 R + 0.5 G + 0.125 B
//
// These can be calculated quickly as
//
//     Y = (R+R+B+G+G+G)/6
//
//     Y = (R+R+R+B+G+G+G+G)>>3

inline auto GetLuma(const Pixel& color) -> uint32_t
{
  const uint32_t r = color.R();
  const uint32_t g = color.G();
  const uint32_t b = color.B();
  return (r + r + b + g + g + g) >> 3;
}

inline GammaCorrection::GammaCorrection(const float gamma, const float thresh)
  : m_gammaReciprocal(1.0F / gamma), m_threshold(thresh)
{
}

inline auto GammaCorrection::GetCorrection(float brightness, const Pixel& color) const -> Pixel
{
  if (brightness < m_threshold)
  {
    return GetBrighterColor(brightness, color, true);
  }
  return GetBrighterColor(std::pow(brightness, m_gammaReciprocal), color, true);
}

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif

#endif
