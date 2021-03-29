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
static const std::vector<vivid::srgb_t> Pastel1
{
  {   0.98431f,   0.70588f,   0.68235f },
  {   0.70196f,   0.80392f,   0.89020f },
  {   0.80000f,   0.92157f,   0.77255f },
  {   0.87059f,   0.79608f,   0.89412f },
  {   0.99608f,   0.85098f,   0.65098f },
  {   1.00000f,   1.00000f,   0.80000f },
  {   0.89804f,   0.84706f,   0.74118f },
  {   0.99216f,   0.85490f,   0.92549f },
  {   0.94902f,   0.94902f,   0.94902f },
};
// clang-format on

#if __cplusplus > 201402L
} // namespace GOOM::UTILS::COLOR_DATA
#else
} // namespace COLOR_DATA
} // namespace UTILS
} // namespace GOOM
#endif
