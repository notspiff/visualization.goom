#pragma once

#include "colordata/colormap_enums.h"
#include "vivid/types.h"

#include <array>
#include <string>
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

    // array of raw maps matching elements of enum 'ColorMapName'
    extern const std::array<std::pair<ColorMapName, std::vector<vivid::srgb_t>>, 89>
        allMaps;

extern const std::vector<ColorMapName> perc_unif_sequentialMaps;
extern const std::vector<ColorMapName> sequentialMaps;
extern const std::vector<ColorMapName> sequential2Maps;
extern const std::vector<ColorMapName> divergingMaps;
extern const std::vector<ColorMapName> diverging_blackMaps;
extern const std::vector<ColorMapName> qualitativeMaps;
extern const std::vector<ColorMapName> miscMaps;
extern const std::vector<ColorMapName> cyclicMaps;

#if __cplusplus > 201402L
} // namespace GOOM::UTILS::COLOR_DATA
#else
} // namespace COLOR_DATA
} // namespace UTILS
} // namespace GOOM
#endif
