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
static const std::vector<vivid::srgb_t> tab20b
{
  {   0.22353f,   0.23137f,   0.47451f },
  {   0.32157f,   0.32941f,   0.63922f },
  {   0.41961f,   0.43137f,   0.81176f },
  {   0.61176f,   0.61961f,   0.87059f },
  {   0.38824f,   0.47451f,   0.22353f },
  {   0.54902f,   0.63529f,   0.32157f },
  {   0.70980f,   0.81176f,   0.41961f },
  {   0.80784f,   0.85882f,   0.61176f },
  {   0.54902f,   0.42745f,   0.19216f },
  {   0.74118f,   0.61961f,   0.22353f },
  {   0.90588f,   0.72941f,   0.32157f },
  {   0.90588f,   0.79608f,   0.58039f },
  {   0.51765f,   0.23529f,   0.22353f },
  {   0.67843f,   0.28627f,   0.29020f },
  {   0.83922f,   0.38039f,   0.41961f },
  {   0.90588f,   0.58824f,   0.61176f },
  {   0.48235f,   0.25490f,   0.45098f },
  {   0.64706f,   0.31765f,   0.58039f },
  {   0.80784f,   0.42745f,   0.74118f },
  {   0.87059f,   0.61961f,   0.83922f },
};
// clang-format on

#if __cplusplus > 201402L
} // namespace GOOM::UTILS::COLOR_DATA
#else
} // namespace COLOR_DATA
} // namespace UTILS
} // namespace GOOM
#endif
