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
static const std::vector<vivid::srgb_t> Set3
{
  {   0.55294f,   0.82745f,   0.78039f },
  {   1.00000f,   1.00000f,   0.70196f },
  {   0.74510f,   0.72941f,   0.85490f },
  {   0.98431f,   0.50196f,   0.44706f },
  {   0.50196f,   0.69412f,   0.82745f },
  {   0.99216f,   0.70588f,   0.38431f },
  {   0.70196f,   0.87059f,   0.41176f },
  {   0.98824f,   0.80392f,   0.89804f },
  {   0.85098f,   0.85098f,   0.85098f },
  {   0.73725f,   0.50196f,   0.74118f },
  {   0.80000f,   0.92157f,   0.77255f },
  {   1.00000f,   0.92941f,   0.43529f },
};
// clang-format on

#if __cplusplus > 201402L
} // namespace GOOM::UTILS::COLOR_DATA
#else
} // namespace COLOR_DATA
} // namespace UTILS
} // namespace GOOM
#endif
