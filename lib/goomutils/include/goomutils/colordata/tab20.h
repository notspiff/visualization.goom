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
static const std::vector<vivid::srgb_t> tab20
{
  {   0.12157f,   0.46667f,   0.70588f },
  {   0.68235f,   0.78039f,   0.90980f },
  {   1.00000f,   0.49804f,   0.05490f },
  {   1.00000f,   0.73333f,   0.47059f },
  {   0.17255f,   0.62745f,   0.17255f },
  {   0.59608f,   0.87451f,   0.54118f },
  {   0.83922f,   0.15294f,   0.15686f },
  {   1.00000f,   0.59608f,   0.58824f },
  {   0.58039f,   0.40392f,   0.74118f },
  {   0.77255f,   0.69020f,   0.83529f },
  {   0.54902f,   0.33725f,   0.29412f },
  {   0.76863f,   0.61176f,   0.58039f },
  {   0.89020f,   0.46667f,   0.76078f },
  {   0.96863f,   0.71373f,   0.82353f },
  {   0.49804f,   0.49804f,   0.49804f },
  {   0.78039f,   0.78039f,   0.78039f },
  {   0.73725f,   0.74118f,   0.13333f },
  {   0.85882f,   0.85882f,   0.55294f },
  {   0.09020f,   0.74510f,   0.81176f },
  {   0.61961f,   0.85490f,   0.89804f },
};
// clang-format on

#if __cplusplus > 201402L
} // namespace GOOM::UTILS::COLOR_DATA
#else
} // namespace COLOR_DATA
} // namespace UTILS
} // namespace GOOM
#endif
