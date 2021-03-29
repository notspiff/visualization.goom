#ifndef LIB_GOOMUTILS_INCLUDE_GOOMUTILS_LOGGING_CONTROL_H_
#define LIB_GOOMUTILS_INCLUDE_GOOMUTILS_LOGGING_CONTROL_H_

#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

#ifndef FORCE_LOGGING
#define NO_LOGGING
#endif

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
#endif
