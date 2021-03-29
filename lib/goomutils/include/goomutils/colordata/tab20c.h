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
static const std::vector<vivid::srgb_t> tab20c
{
  {   0.19216f,   0.50980f,   0.74118f },
  {   0.41961f,   0.68235f,   0.83922f },
  {   0.61961f,   0.79216f,   0.88235f },
  {   0.77647f,   0.85882f,   0.93725f },
  {   0.90196f,   0.33333f,   0.05098f },
  {   0.99216f,   0.55294f,   0.23529f },
  {   0.99216f,   0.68235f,   0.41961f },
  {   0.99216f,   0.81569f,   0.63529f },
  {   0.19216f,   0.63922f,   0.32941f },
  {   0.45490f,   0.76863f,   0.46275f },
  {   0.63137f,   0.85098f,   0.60784f },
  {   0.78039f,   0.91373f,   0.75294f },
  {   0.45882f,   0.41961f,   0.69412f },
  {   0.61961f,   0.60392f,   0.78431f },
  {   0.73725f,   0.74118f,   0.86275f },
  {   0.85490f,   0.85490f,   0.92157f },
  {   0.38824f,   0.38824f,   0.38824f },
  {   0.58824f,   0.58824f,   0.58824f },
  {   0.74118f,   0.74118f,   0.74118f },
  {   0.85098f,   0.85098f,   0.85098f },
};
// clang-format on

#if __cplusplus > 201402L
} // namespace GOOM::UTILS::COLOR_DATA
#else
} // namespace COLOR_DATA
} // namespace UTILS
} // namespace GOOM
#endif
