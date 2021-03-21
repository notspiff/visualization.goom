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
static const std::vector<vivid::srgb_t> Set1
{
  {   0.89412f,   0.10196f,   0.10980f },
  {   0.21569f,   0.49412f,   0.72157f },
  {   0.30196f,   0.68627f,   0.29020f },
  {   0.59608f,   0.30588f,   0.63922f },
  {   1.00000f,   0.49804f,   0.00000f },
  {   1.00000f,   1.00000f,   0.20000f },
  {   0.65098f,   0.33725f,   0.15686f },
  {   0.96863f,   0.50588f,   0.74902f },
  {   0.60000f,   0.60000f,   0.60000f },
};
// clang-format on

#if __cplusplus > 201402L
} // namespace GOOM::UTILS::COLOR_DATA
#else
} // namespace COLOR_DATA
} // namespace UTILS
} // namespace GOOM
#endif
