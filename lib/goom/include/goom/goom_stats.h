#ifndef VISUALIZATION_GOOM_GOOM_STATS_H
#define VISUALIZATION_GOOM_GOOM_STATS_H

#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#if __cplusplus > 201402L
#include <variant>
#endif

namespace GOOM
{

class GoomStats
{
public:
#if __cplusplus <= 201402L
  struct StatsValue
  {
    StatsValue(std::string s_val) : type{Type::STRING}, str{std::move(s_val)} {}
    StatsValue(const uint32_t i) : type{Type::UINT32}, ui32{i} {}
    StatsValue(const int32_t i) : type{Type::INT32}, i32{i} {}
    StatsValue(const uint64_t i) : type{Type::UINT64}, ui64{i} {}
    StatsValue(const float f) : type{Type::FLOAT}, flt{f} {}
    enum class Type
    {
      STRING,
      UINT32,
      INT32,
      UINT64,
      FLOAT
    } type;
    std::string str{};
    uint32_t ui32{};
    int32_t i32{};
    uint64_t ui64{};
    float flt{};
  };
#else
  using StatsValue = std::variant<std::string, uint32_t, int32_t, uint64_t, float>;
#endif
  using WriteValueFunc = std::function<void(const std::string&)>;

  explicit GoomStats(WriteValueFunc writeValueFunc);

  using LogStatsValueFunc =
      std::function<void(const std::string& module, const std::string& name, const StatsValue&)>;
  auto GetLogStatsValueFunc() const -> LogStatsValueFunc;

private:
  const WriteValueFunc m_writeValue;
  void LogStatsValue(const std::string& module,
                     const std::string& name,
                     const StatsValue& statsValue) const;
};

} // namespace GOOM
#endif //VISUALIZATION_GOOM_GOOM_STATS_H
