#include "goom_stats.h"

#include <cstdint>
#include <format>
#include <stdexcept>
#include <string>
#include <utility>

namespace GOOM
{

GoomStats::GoomStats(WriteValueFunc writeValueFunc) : m_writeValue{std::move(writeValueFunc)}
{
}

auto GoomStats::GetLogStatsValueFunc() const -> LogStatsValueFunc
{
  const auto logStatsValue = [this](const std::string& module, const std::string& name,
                                    const StatsValue& statsValue) {
    LogStatsValue(module, name, statsValue);
  };

  return logStatsValue;
}

class LogStatsVisitor
{
public:
  LogStatsVisitor(const std::string& m, const std::string& n) : m_module{m}, m_name{n} {}

  std::string operator()(const std::string& s) const
  {
    return std20::format("{}.{} = '{}'", m_module, m_name, s);
  }
  std::string operator()(const uint32_t i) const
  {
    return std20::format("{}.{} = {}", m_module, m_name, i);
  }
  std::string operator()(const int32_t i) const
  {
    return std20::format("{}.{} = {}", m_module, m_name, i);
  }
  std::string operator()(const uint64_t i) const
  {
    return std20::format("{}.{} = {}", m_module, m_name, i);
  }
  std::string operator()(const float f) const
  {
    return std20::format("{}.{} = {:.3}", m_module, m_name, f);
  }

private:
  const std::string& m_module;
  const std::string& m_name;
};

void GoomStats::LogStatsValue(const std::string& module,
                              const std::string& name,
                              const StatsValue& statsValue) const
{
  if (!m_writeValue)
  {
    throw std::logic_error("WriteValueFunc has not been set.");
  }

#if __cplusplus <= 201402L
  const LogStatsVisitor visit{module, name};
  std::string valueStr{};
  switch (statsValue.type)
  {
    case StatsValue::Type::STRING:
      valueStr = visit(statsValue.str);
      break;
    case StatsValue::Type::UINT32:
      valueStr = visit(statsValue.ui32);
      break;
    case StatsValue::Type::INT32:
      valueStr = visit(statsValue.i32);
      break;
    case StatsValue::Type::UINT64:
      valueStr = visit(statsValue.ui64);
      break;
    case StatsValue::Type::FLOAT:
      valueStr = visit(statsValue.flt);
      break;
  }
#else
  valueStr = std::visit(LogStatsVisitor{module, name}, statsValue);
#endif
  m_writeValue(valueStr);
}

} // namespace GOOM
