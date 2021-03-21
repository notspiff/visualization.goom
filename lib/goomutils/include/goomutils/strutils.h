#ifndef VISUALIZATION_GOOM_LIB_GOOMUTILS_STRUTILS_H_
#define VISUALIZATION_GOOM_LIB_GOOMUTILS_STRUTILS_H_

#if __cplusplus > 201402L
#include <magic_enum.hpp>
#endif
#include <stdexcept>
#include <string>
#include <vector>

#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

auto SplitString(const std::string& str, const std::string& delim) -> std::vector<std::string>;

template<class E>
auto EnumToString(const E e) -> std::string
{
#if __cplusplus <= 201402L
  return std::to_string(static_cast<int>(e));
#else
  return std::string(magic_enum::enum_name(e));
#endif
}

#if __cplusplus > 201402L
template<class E>
auto StringToEnum(const std::string& eStr) -> E
{
  const auto val = magic_enum::enum_cast<E>(eStr);
  if (val)
  {
    return *val;
  }

  throw std::runtime_error("Unknown enum value \"" + eStr + "\".");
}
#endif

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
#endif
