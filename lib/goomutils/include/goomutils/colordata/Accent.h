#pragma once

#include "vivid/types.h"
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
static const std::vector<vivid::srgb_t> Accent
{
  {   0.49804f,   0.78824f,   0.49804f },
  {   0.74510f,   0.68235f,   0.83137f },
  {   0.99216f,   0.75294f,   0.52549f },
  {   1.00000f,   1.00000f,   0.60000f },
  {   0.21961f,   0.42353f,   0.69020f },
  {   0.94118f,   0.00784f,   0.49804f },
  {   0.74902f,   0.35686f,   0.09020f },
  {   0.40000f,   0.40000f,   0.40000f },
};
// clang-format on

#if __cplusplus > 201402L
} // namespace GOOM::UTILS::COLOR_DATA
#else
} // namespace COLOR_DATA
} // namespace UTILS
} // namespace GOOM
#endif
