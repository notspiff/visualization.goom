#include "catch2/catch.hpp"
#include "colorutils.h"
#include "goom_graphic.h"

#include <algorithm>

using namespace GOOM;
using namespace UTILS;

TEST_CASE("Test max channels", "[channels-max]")
{
  REQUIRE(channel_limits<uint8_t>::min() == 0);
  REQUIRE(channel_limits<uint8_t>::max() == 255);
  REQUIRE(channel_limits<uint32_t>::min() == 0);
  REQUIRE(channel_limits<uint32_t>::max() == 255);
  REQUIRE(channel_limits<int>::min() == 0);
  REQUIRE(channel_limits<int>::max() == 255);
  REQUIRE(channel_limits<float>::min() == 0);
  REQUIRE(channel_limits<float>::max() == 255.0F);
}

TEST_CASE("Color channels are added", "[color-channel-add]")
{
  REQUIRE(ColorChannelAdd(100, 120) == 220);
  REQUIRE(ColorChannelAdd(200, 120) == 320);
  REQUIRE(ColorChannelAdd(0, 120) == 120);
  REQUIRE(ColorChannelAdd(0, 0) == 0);
}

TEST_CASE("Colors are added", "[color-add]")
{
#if __cplusplus <= 201402L
  const Pixel c1{{100, 50, 20}};
  const Pixel c2{{120, 250, 70}};
#else
  const Pixel c1{{.r = 100, .g = 50, .b = 20}};
  const Pixel c2{{.r = 120, .g = 250, .b = 70}};
#endif
  const Pixel c3 = GetColorAdd(c1, c2, true);

  REQUIRE(static_cast<uint32_t>(c3.R()) == 220);
  REQUIRE(static_cast<uint32_t>(c3.G()) == 255);
  REQUIRE(static_cast<uint32_t>(c3.B()) == 90);

  const Pixel c4 = GetColorAdd(c1, c2, false);
  REQUIRE(static_cast<uint32_t>(c4.R()) == 220 * 255 / 300);
  REQUIRE(static_cast<uint32_t>(c4.G()) == 255);
  REQUIRE(static_cast<uint32_t>(c4.B()) == 90 * 255 / 300);
}

TEST_CASE("Color channels are brightened", "[color-channel-bright]")
{
  REQUIRE(GetBrighterChannelColor(100, 2) == 100 * 2 / 255);
  REQUIRE(GetBrighterChannelColor(11, 20) == 11 * 20 / 255);
  REQUIRE(GetBrighterChannelColor(0, 20) == 0);
  REQUIRE(GetBrighterChannelColor(100, 20) == stdnew::clamp(0U, 100U * 20U / 255U, 255U));
}

TEST_CASE("Colors are brightened", "[color-bright]")
{
#if __cplusplus <= 201402L
  const Pixel c{{100, 50, 20}};
#else
  const Pixel c{{.r = 100, .g = 50, .b = 20}};
#endif

  Pixel cb = GetBrighterColor(1.0F, c, false);
  REQUIRE(cb.R() == 100);
  REQUIRE(cb.G() == 50);
  REQUIRE(cb.B() == 20);

  cb = GetBrighterColor(0.5F, c, false);
  REQUIRE(cb.R() == 50);
  REQUIRE(cb.G() == 25);
  REQUIRE(cb.B() == 10);

  cb = GetBrighterColor(0.01F, c, false);
  REQUIRE(cb.R() == 1);
  REQUIRE(cb.G() == 0);
  REQUIRE(cb.B() == 0);
}

TEST_CASE("Half intensity color", "[color-half-intensity]")
{
#if __cplusplus <= 201402L
  const Pixel c{{100, 50, 20}};
#else
  const Pixel c{{.r = 100, .g = 50, .b = 20}};
#endif
  const Pixel ch = GetHalfIntensityColor(c);

  REQUIRE(ch.R() == 50);
  REQUIRE(ch.G() == 25);
  REQUIRE(ch.B() == 10);
}

TEST_CASE("Lighten", "[color-lighten]")
{
#if __cplusplus <= 201402L
  const Pixel c{{100, 0, 0}};
#else
  const Pixel c{{.r = 100, .g = 0, .b = 0}};
#endif

  const Pixel cl = GetLightenedColor(c, 10.0);
  REQUIRE(static_cast<uint32_t>(cl.R()) == 50);
  REQUIRE(static_cast<uint32_t>(cl.G()) == 0);
  REQUIRE(static_cast<uint32_t>(cl.B()) == 0);
}

TEST_CASE("Lightened color", "[color-half-lightened]")
{
#if __cplusplus <= 201402L
  const Pixel c{{100, 50, 20}};
#else
  const Pixel c{{.r = 100, .g = 50, .b = 20}};
#endif

  Pixel cl = GetLightenedColor(c, 0.5);
  REQUIRE(cl.R() == 0);
  REQUIRE(cl.G() == 0);
  REQUIRE(cl.B() == 0);

  cl = GetLightenedColor(c, 1.0);
  REQUIRE(cl.R() == 0);
  REQUIRE(cl.G() == 0);
  REQUIRE(cl.B() == 0);

  cl = GetLightenedColor(c, 2.0);
  REQUIRE(cl.R() == 15);
  REQUIRE(cl.G() == 7);
  REQUIRE(cl.B() == 3);

  cl = GetLightenedColor(c, 5.0);
  REQUIRE(cl.R() == 34);
  REQUIRE(cl.G() == 17);
  REQUIRE(cl.B() == 6);

  cl = GetLightenedColor(c, 10.0);
  REQUIRE(cl.R() == 50);
  REQUIRE(cl.G() == 25);
  REQUIRE(cl.B() == 10);

  const Pixel c2 = Pixel::WHITE;
  cl = GetLightenedColor(c2, 1.0);
  REQUIRE(cl.R() == 0);
  REQUIRE(cl.G() == 0);
  REQUIRE(cl.B() == 0);

  cl = GetLightenedColor(c2, 2.0);
  REQUIRE(cl.R() == 38);
  REQUIRE(cl.G() == 38);
  REQUIRE(cl.B() == 38);

  cl = GetLightenedColor(c2, 5.0);
  REQUIRE(cl.R() == 89);
  REQUIRE(cl.G() == 89);
  REQUIRE(cl.B() == 89);

  cl = GetLightenedColor(c2, 10.0);
  REQUIRE(cl.R() == 127);
  REQUIRE(cl.G() == 127);
  REQUIRE(cl.B() == 127);
}

TEST_CASE("Evolved color", "[color-evolve]")
{
#if __cplusplus <= 201402L
  const Pixel c{{100, 50, 20}};
#else
  const Pixel c{{.r = 100, .g = 50, .b = 20}};
#endif
  Pixel cl;

  cl = GetEvolvedColor(c);
  REQUIRE(cl.R() == 67);
  REQUIRE(cl.G() == 33);
  REQUIRE(cl.B() == 13);

  cl = GetEvolvedColor(cl);
  REQUIRE(cl.R() == 44);
  REQUIRE(cl.G() == 22);
  REQUIRE(cl.B() == 8);

  cl = GetEvolvedColor(cl);
  REQUIRE(cl.R() == 29);
  REQUIRE(cl.G() == 14);
  REQUIRE(cl.B() == 5);

  cl = GetEvolvedColor(cl);
  REQUIRE(cl.R() == 19);
  REQUIRE(cl.G() == 9);
  REQUIRE(cl.B() == 3);
}
