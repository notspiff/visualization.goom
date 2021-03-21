#ifndef _GOOM_TESTING_H
#define _GOOM_TESTING_H

#include <cstdint>
#include <string>

namespace GOOM
{

constexpr auto GOOM_VER_NAME = "Goom Library";
constexpr auto GOOM_VER_DESCRIPTION = "Goom++ Initial Release";
constexpr auto GOOM_VER_MAJOR = 1;
constexpr auto GOOM_VER_MINOR = 0;
constexpr auto GOOM_VER_REVISION = 0;

struct VersionInfoInt
{
  int32_t verMajor;
  int32_t verMinor;
  int32_t verRevision;
};

inline auto GetFullVersionStr() -> std::string
{
  return std::string(GOOM_VER_NAME) + " " + std::to_string(GOOM_VER_MAJOR) + "." +
         std::to_string(GOOM_VER_MINOR) + "." + std::to_string(GOOM_VER_REVISION) + "," + " " +
         GOOM_VER_DESCRIPTION;
}

inline auto GetVersionInfo() -> VersionInfoInt
{
  return {GOOM_VER_MAJOR, GOOM_VER_MINOR, GOOM_VER_REVISION};
}

} // namespace GOOM
#endif
