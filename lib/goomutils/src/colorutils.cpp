#include "colorutils.h"

#include "goom/goom_graphic.h"
#if __cplusplus <= 201402L
#include "goomutils//mathutils.h"
#endif

#include <algorithm>
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

static_assert(sizeof(Pixel) == sizeof(uint32_t), "Invalid Pixel size.");

auto GetIntColor(uint8_t r, uint8_t g, uint8_t b) -> Pixel
{
  return Pixel{{/*.r = */ r, /*.g = */ g, /*.b = */ b, /*.a = */ 0xff}};
}

inline auto Lighten(const uint8_t value, const float power) -> uint8_t
{
  const auto t = static_cast<float>(value) * std::log10(power) / 2.0F;
  if (t <= 0.0)
  {
    return 0;
  }

  // (32.0f * log (t));
  return stdnew::clamp(static_cast<int>(t), channel_limits<int>::min(), channel_limits<int>::max());
}

auto GetLightenedColor(const Pixel& oldColor, float power) -> Pixel
{
  Pixel pixel = oldColor;

  pixel.SetR(Lighten(pixel.R(), power));
  pixel.SetG(Lighten(pixel.G(), power));
  pixel.SetB(Lighten(pixel.B(), power));

  return pixel;
}

inline auto EvolvedColor(const Pixel& src,
                         const Pixel& dest,
                         const uint32_t mask,
                         const uint32_t incr) -> Pixel
{
  const auto color = static_cast<int32_t>(src.Rgba() & (~mask));
  uint32_t isrc = src.Rgba() & mask;
  const uint32_t idest = dest.Rgba() & mask;

  if ((isrc != mask) && (isrc < idest))
  {
    isrc += incr;
  }
  if (isrc > idest)
  {
    isrc -= incr;
  }

  return Pixel{static_cast<uint32_t>((isrc & mask) | static_cast<uint32_t>(color))};
}

auto GetEvolvedColor(const Pixel& baseColor) -> Pixel
{
  Pixel newColor = baseColor;

  newColor = EvolvedColor(newColor, baseColor, 0xff, 0x01);
  newColor = EvolvedColor(newColor, baseColor, 0xff00, 0x0100);
  newColor = EvolvedColor(newColor, baseColor, 0xff0000, 0x010000);
  newColor = EvolvedColor(newColor, baseColor, 0xff000000, 0x01000000);

  newColor = GetLightenedColor(newColor, 10.0 * 2.0F + 2.0F);

  return newColor;
}

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
