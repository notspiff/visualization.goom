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
static const std::vector<vivid::srgb_t> Set2
{
  {   0.40000f,   0.76078f,   0.64706f },
  {   0.98824f,   0.55294f,   0.38431f },
  {   0.55294f,   0.62745f,   0.79608f },
  {   0.90588f,   0.54118f,   0.76471f },
  {   0.65098f,   0.84706f,   0.32941f },
  {   1.00000f,   0.85098f,   0.18431f },
  {   0.89804f,   0.76863f,   0.58039f },
  {   0.70196f,   0.70196f,   0.70196f },
};
// clang-format on

#if __cplusplus > 201402L
} // namespace GOOM::UTILS::COLOR_DATA
#else
} // namespace COLOR_DATA
} // namespace UTILS
} // namespace GOOM
#endif
