#pragma once

#if __cplusplus > 201402L
namespace GOOM::UTILS::COLOR_DATA
{
#else
namespace GOOM
{
namespace UTILS
{
namespace COLOR_DATA
{
#endif

    enum class ColorMapName {
      Accent,
      afmhot,
      autumn,
      binary,
      Blues,
      bone,
      BrBG,
      brg,
      BuGn,
      BuPu,
      bwr,
      cividis,
      CMRmap,
      cool,
      coolwarm,
      copper,
      cubehelix,
      Dark2,
      flag,
      gist_earth,
      gist_gray,
      gist_heat,
      gist_ncar,
      gist_rainbow,
      gist_stern,
      gist_yarg,
      GnBu,
      gnuplot,
      gnuplot2,
      gray,
      Greens,
      Greys,
      hot,
      hsv,
      inferno,
      jet,
      magma,
      nipy_spectral,
      ocean,
      Oranges,
      OrRd,
      Paired,
      Pastel1,
      Pastel2,
      pink,
      pink_black_green_w3c_,
      PiYG,
      plasma,
      PRGn,
      prism,
      PuBu,
      PuBuGn,
      PuOr,
      PuRd,
      Purples,
      rainbow,
      RdBu,
      RdGy,
      RdPu,
      RdYlBu,
      RdYlGn,
      red_black_blue,
      red_black_green,
      red_black_orange,
      red_black_sky,
      Reds,
      seismic,
      Set1,
      Set2,
      Set3,
      Spectral,
      spring,
      summer,
      tab10,
      tab20,
      tab20b,
      tab20c,
      terrain,
      twilight,
      twilight_shifted,
      viridis,
      winter,
      Wistia,
      yellow_black_blue,
      yellow_black_sky,
      YlGn,
      YlGnBu,
      YlOrBr,
      YlOrRd,
      _NULL,
    };

#if __cplusplus > 201402L
} // namespace GOOM::UTILS::COLOR_DATA
#else
} // namespace COLOR_DATA
} // namespace UTILS
} // namespace GOOM
#endif
