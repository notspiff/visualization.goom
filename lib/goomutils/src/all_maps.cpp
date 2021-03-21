#include "colordata/all_maps.h"

// clang-format off
#include "colordata/Accent.h"
#include "colordata/afmhot.h"
#include "colordata/autumn.h"
#include "colordata/binary.h"
#include "colordata/Blues.h"
#include "colordata/bone.h"
#include "colordata/BrBG.h"
#include "colordata/brg.h"
#include "colordata/BuGn.h"
#include "colordata/BuPu.h"
#include "colordata/bwr.h"
#include "colordata/cividis.h"
#include "colordata/CMRmap.h"
#include "colordata/cool.h"
#include "colordata/coolwarm.h"
#include "colordata/copper.h"
#include "colordata/cubehelix.h"
#include "colordata/Dark2.h"
#include "colordata/flag.h"
#include "colordata/gist_earth.h"
#include "colordata/gist_gray.h"
#include "colordata/gist_heat.h"
#include "colordata/gist_ncar.h"
#include "colordata/gist_rainbow.h"
#include "colordata/gist_stern.h"
#include "colordata/gist_yarg.h"
#include "colordata/GnBu.h"
#include "colordata/gnuplot.h"
#include "colordata/gnuplot2.h"
#include "colordata/gray.h"
#include "colordata/Greens.h"
#include "colordata/Greys.h"
#include "colordata/hot.h"
#include "colordata/hsv.h"
#include "colordata/inferno.h"
#include "colordata/jet.h"
#include "colordata/magma.h"
#include "colordata/nipy_spectral.h"
#include "colordata/ocean.h"
#include "colordata/Oranges.h"
#include "colordata/OrRd.h"
#include "colordata/Paired.h"
#include "colordata/Pastel1.h"
#include "colordata/Pastel2.h"
#include "colordata/pink.h"
#include "colordata/pink_black_green_w3c_.h"
#include "colordata/PiYG.h"
#include "colordata/plasma.h"
#include "colordata/PRGn.h"
#include "colordata/prism.h"
#include "colordata/PuBu.h"
#include "colordata/PuBuGn.h"
#include "colordata/PuOr.h"
#include "colordata/PuRd.h"
#include "colordata/Purples.h"
#include "colordata/rainbow.h"
#include "colordata/RdBu.h"
#include "colordata/RdGy.h"
#include "colordata/RdPu.h"
#include "colordata/RdYlBu.h"
#include "colordata/RdYlGn.h"
#include "colordata/red_black_blue.h"
#include "colordata/red_black_green.h"
#include "colordata/red_black_orange.h"
#include "colordata/red_black_sky.h"
#include "colordata/Reds.h"
#include "colordata/seismic.h"
#include "colordata/Set1.h"
#include "colordata/Set2.h"
#include "colordata/Set3.h"
#include "colordata/Spectral.h"
#include "colordata/spring.h"
#include "colordata/summer.h"
#include "colordata/tab10.h"
#include "colordata/tab20.h"
#include "colordata/tab20b.h"
#include "colordata/tab20c.h"
#include "colordata/terrain.h"
#include "colordata/twilight.h"
#include "colordata/twilight_shifted.h"
#include "colordata/viridis.h"
#include "colordata/winter.h"
#include "colordata/Wistia.h"
#include "colordata/yellow_black_blue.h"
#include "colordata/yellow_black_sky.h"
#include "colordata/YlGn.h"
#include "colordata/YlGnBu.h"
#include "colordata/YlOrBr.h"
#include "colordata/YlOrRd.h"
// clang-format on

#include "colordata/colormap_enums.h"
#include "vivid/types.h"

#include <array>
#include <utility>
#include <vector>

#if __cplusplus > 201402L
namespace GOOM::UTILS::COLOR_DATA
#else
namespace GOOM
{
namespace UTILS
{
namespace COLOR_DATA
{
#endif

    // clang-format off
const std::array<std::pair<ColorMapName, std::vector<vivid::srgb_t>>, 89> allMaps
{
  std::make_pair(ColorMapName::Accent, COLOR_DATA::Accent),
  std::make_pair(ColorMapName::afmhot, COLOR_DATA::afmhot),
  std::make_pair(ColorMapName::autumn, COLOR_DATA::autumn),
  std::make_pair(ColorMapName::binary, COLOR_DATA::binary),
  std::make_pair(ColorMapName::Blues, COLOR_DATA::Blues),
  std::make_pair(ColorMapName::bone, COLOR_DATA::bone),
  std::make_pair(ColorMapName::BrBG, COLOR_DATA::BrBG),
  std::make_pair(ColorMapName::brg, COLOR_DATA::brg),
  std::make_pair(ColorMapName::BuGn, COLOR_DATA::BuGn),
  std::make_pair(ColorMapName::BuPu, COLOR_DATA::BuPu),
  std::make_pair(ColorMapName::bwr, COLOR_DATA::bwr),
  std::make_pair(ColorMapName::cividis, COLOR_DATA::cividis),
  std::make_pair(ColorMapName::CMRmap, COLOR_DATA::CMRmap),
  std::make_pair(ColorMapName::cool, COLOR_DATA::cool),
  std::make_pair(ColorMapName::coolwarm, COLOR_DATA::coolwarm),
  std::make_pair(ColorMapName::copper, COLOR_DATA::copper),
  std::make_pair(ColorMapName::cubehelix, COLOR_DATA::cubehelix),
  std::make_pair(ColorMapName::Dark2, COLOR_DATA::Dark2),
  std::make_pair(ColorMapName::flag, COLOR_DATA::flag),
  std::make_pair(ColorMapName::gist_earth, COLOR_DATA::gist_earth),
  std::make_pair(ColorMapName::gist_gray, COLOR_DATA::gist_gray),
  std::make_pair(ColorMapName::gist_heat, COLOR_DATA::gist_heat),
  std::make_pair(ColorMapName::gist_ncar, COLOR_DATA::gist_ncar),
  std::make_pair(ColorMapName::gist_rainbow, COLOR_DATA::gist_rainbow),
  std::make_pair(ColorMapName::gist_stern, COLOR_DATA::gist_stern),
  std::make_pair(ColorMapName::gist_yarg, COLOR_DATA::gist_yarg),
  std::make_pair(ColorMapName::GnBu, COLOR_DATA::GnBu),
  std::make_pair(ColorMapName::gnuplot, COLOR_DATA::gnuplot),
  std::make_pair(ColorMapName::gnuplot2, COLOR_DATA::gnuplot2),
  std::make_pair(ColorMapName::gray, COLOR_DATA::gray),
  std::make_pair(ColorMapName::Greens, COLOR_DATA::Greens),
  std::make_pair(ColorMapName::Greys, COLOR_DATA::Greys),
  std::make_pair(ColorMapName::hot, COLOR_DATA::hot),
  std::make_pair(ColorMapName::hsv, COLOR_DATA::hsv),
  std::make_pair(ColorMapName::inferno, COLOR_DATA::inferno),
  std::make_pair(ColorMapName::jet, COLOR_DATA::jet),
  std::make_pair(ColorMapName::magma, COLOR_DATA::magma),
  std::make_pair(ColorMapName::nipy_spectral, COLOR_DATA::nipy_spectral),
  std::make_pair(ColorMapName::ocean, COLOR_DATA::ocean),
  std::make_pair(ColorMapName::Oranges, COLOR_DATA::Oranges),
  std::make_pair(ColorMapName::OrRd, COLOR_DATA::OrRd),
  std::make_pair(ColorMapName::Paired, COLOR_DATA::Paired),
  std::make_pair(ColorMapName::Pastel1, COLOR_DATA::Pastel1),
  std::make_pair(ColorMapName::Pastel2, COLOR_DATA::Pastel2),
  std::make_pair(ColorMapName::pink, COLOR_DATA::pink),
  std::make_pair(ColorMapName::pink_black_green_w3c_, COLOR_DATA::pink_black_green_w3c_),
  std::make_pair(ColorMapName::PiYG, COLOR_DATA::PiYG),
  std::make_pair(ColorMapName::plasma, COLOR_DATA::plasma),
  std::make_pair(ColorMapName::PRGn, COLOR_DATA::PRGn),
  std::make_pair(ColorMapName::prism, COLOR_DATA::prism),
  std::make_pair(ColorMapName::PuBu, COLOR_DATA::PuBu),
  std::make_pair(ColorMapName::PuBuGn, COLOR_DATA::PuBuGn),
  std::make_pair(ColorMapName::PuOr, COLOR_DATA::PuOr),
  std::make_pair(ColorMapName::PuRd, COLOR_DATA::PuRd),
  std::make_pair(ColorMapName::Purples, COLOR_DATA::Purples),
  std::make_pair(ColorMapName::rainbow, COLOR_DATA::rainbow),
  std::make_pair(ColorMapName::RdBu, COLOR_DATA::RdBu),
  std::make_pair(ColorMapName::RdGy, COLOR_DATA::RdGy),
  std::make_pair(ColorMapName::RdPu, COLOR_DATA::RdPu),
  std::make_pair(ColorMapName::RdYlBu, COLOR_DATA::RdYlBu),
  std::make_pair(ColorMapName::RdYlGn, COLOR_DATA::RdYlGn),
  std::make_pair(ColorMapName::red_black_blue, COLOR_DATA::red_black_blue),
  std::make_pair(ColorMapName::red_black_green, COLOR_DATA::red_black_green),
  std::make_pair(ColorMapName::red_black_orange, COLOR_DATA::red_black_orange),
  std::make_pair(ColorMapName::red_black_sky, COLOR_DATA::red_black_sky),
  std::make_pair(ColorMapName::Reds, COLOR_DATA::Reds),
  std::make_pair(ColorMapName::seismic, COLOR_DATA::seismic),
  std::make_pair(ColorMapName::Set1, COLOR_DATA::Set1),
  std::make_pair(ColorMapName::Set2, COLOR_DATA::Set2),
  std::make_pair(ColorMapName::Set3, COLOR_DATA::Set3),
  std::make_pair(ColorMapName::Spectral, COLOR_DATA::Spectral),
  std::make_pair(ColorMapName::spring, COLOR_DATA::spring),
  std::make_pair(ColorMapName::summer, COLOR_DATA::summer),
  std::make_pair(ColorMapName::tab10, COLOR_DATA::tab10),
  std::make_pair(ColorMapName::tab20, COLOR_DATA::tab20),
  std::make_pair(ColorMapName::tab20b, COLOR_DATA::tab20b),
  std::make_pair(ColorMapName::tab20c, COLOR_DATA::tab20c),
  std::make_pair(ColorMapName::terrain, COLOR_DATA::terrain),
  std::make_pair(ColorMapName::twilight, COLOR_DATA::twilight),
  std::make_pair(ColorMapName::twilight_shifted, COLOR_DATA::twilight_shifted),
  std::make_pair(ColorMapName::viridis, COLOR_DATA::viridis),
  std::make_pair(ColorMapName::winter, COLOR_DATA::winter),
  std::make_pair(ColorMapName::Wistia, COLOR_DATA::Wistia),
  std::make_pair(ColorMapName::yellow_black_blue, COLOR_DATA::yellow_black_blue),
  std::make_pair(ColorMapName::yellow_black_sky, COLOR_DATA::yellow_black_sky),
  std::make_pair(ColorMapName::YlGn, COLOR_DATA::YlGn),
  std::make_pair(ColorMapName::YlGnBu, COLOR_DATA::YlGnBu),
  std::make_pair(ColorMapName::YlOrBr, COLOR_DATA::YlOrBr),
  std::make_pair(ColorMapName::YlOrRd, COLOR_DATA::YlOrRd),
};

const std::vector<ColorMapName> perc_unif_sequentialMaps
{
    ColorMapName::cividis,
    ColorMapName::inferno,
    ColorMapName::magma,
    ColorMapName::plasma,
    ColorMapName::viridis,
};
const std::vector<ColorMapName> sequentialMaps
{
    ColorMapName::Blues,
    ColorMapName::BuGn,
    ColorMapName::BuPu,
    ColorMapName::GnBu,
    ColorMapName::Greens,
    ColorMapName::Greys,
    ColorMapName::Oranges,
    ColorMapName::OrRd,
    ColorMapName::PuBu,
    ColorMapName::PuBuGn,
    ColorMapName::PuRd,
    ColorMapName::Purples,
    ColorMapName::RdPu,
    ColorMapName::Reds,
    ColorMapName::YlGn,
    ColorMapName::YlGnBu,
    ColorMapName::YlOrBr,
    ColorMapName::YlOrRd,
};
const std::vector<ColorMapName> sequential2Maps
{
    ColorMapName::afmhot,
    ColorMapName::autumn,
    ColorMapName::binary,
    ColorMapName::bone,
    ColorMapName::cool,
    ColorMapName::copper,
    ColorMapName::gist_gray,
    ColorMapName::gist_heat,
    ColorMapName::gist_yarg,
    ColorMapName::gray,
    ColorMapName::hot,
    ColorMapName::pink,
    ColorMapName::spring,
    ColorMapName::summer,
    ColorMapName::winter,
    ColorMapName::Wistia,
};
const std::vector<ColorMapName> divergingMaps
{
    ColorMapName::BrBG,
    ColorMapName::bwr,
    ColorMapName::coolwarm,
    ColorMapName::PiYG,
    ColorMapName::PRGn,
    ColorMapName::PuOr,
    ColorMapName::RdBu,
    ColorMapName::RdGy,
    ColorMapName::RdYlBu,
    ColorMapName::RdYlGn,
    ColorMapName::seismic,
    ColorMapName::Spectral,
};
const std::vector<ColorMapName> diverging_blackMaps
{
    ColorMapName::pink_black_green_w3c_,
    ColorMapName::red_black_blue,
    ColorMapName::red_black_green,
    ColorMapName::red_black_orange,
    ColorMapName::red_black_sky,
    ColorMapName::yellow_black_blue,
    ColorMapName::yellow_black_sky,
};
const std::vector<ColorMapName> qualitativeMaps
{
    ColorMapName::Accent,
    ColorMapName::Dark2,
    ColorMapName::Paired,
    ColorMapName::Pastel1,
    ColorMapName::Pastel2,
    ColorMapName::Set1,
    ColorMapName::Set2,
    ColorMapName::Set3,
    ColorMapName::tab10,
    ColorMapName::tab20,
    ColorMapName::tab20b,
    ColorMapName::tab20c,
};
const std::vector<ColorMapName> miscMaps
{
    ColorMapName::brg,
    ColorMapName::CMRmap,
    ColorMapName::cubehelix,
    ColorMapName::flag,
    ColorMapName::gist_earth,
    ColorMapName::gist_ncar,
    ColorMapName::gist_rainbow,
    ColorMapName::gist_stern,
    ColorMapName::gnuplot,
    ColorMapName::gnuplot2,
    ColorMapName::hsv,
    ColorMapName::jet,
    ColorMapName::nipy_spectral,
    ColorMapName::ocean,
    ColorMapName::prism,
    ColorMapName::rainbow,
    ColorMapName::terrain,
};
const std::vector<ColorMapName> cyclicMaps
{
    ColorMapName::hsv,
    ColorMapName::twilight,
    ColorMapName::twilight_shifted,
};
// clang-format on

#if __cplusplus > 201402L
} // namespace GOOM::UTILS::COLOR_DATA
#else
} // namespace COLOR_DATA
} // namespace UTILS
} // namespace GOOM
#endif
