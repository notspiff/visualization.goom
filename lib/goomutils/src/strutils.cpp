#include "goomutils/strutils.h"

#if __cplusplus > 201402L
#include <ranges>
#endif
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

auto SplitString(const std::string& str, const std::string& delim) -> std::vector<std::string>
{
#if __cplusplus <= 201402L
  std::vector<std::string> vec;
  std::string token;
  std::string s = str;
  size_t pos;
  while ((pos = s.find(delim)) != std::string::npos)
  {
    token = s.substr(0, pos);
    s.erase(0, pos + delim.length());
    vec.emplace_back(token);
  }
  if (!s.empty())
  {
    vec.emplace_back(s);
  }
  return vec;
#else
  auto parts = str | std::views::split(delim);
  std::vector<std::string> vec;
  for (auto part : parts)
  {
    std::string s = "";
    for (auto c : part)
    {
      s += c;
    }
    vec.emplace_back(s);
  }
  return vec;
#endif
}

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
