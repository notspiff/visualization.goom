#ifndef VISUALIZATION_GOOM_STATS_LINE_STATS_H
#define VISUALIZATION_GOOM_STATS_LINE_STATS_H

#include "goom/goom_config.h"
#include "goom/goom_stats.h"

#include <chrono>
#include <cstdint>
#include <string>

namespace GOOM
{

class LineStats
{
public:
  LineStats() = default;

  void Reset();
  void Log(const GoomStats::LogStatsValueFunc& l) const;

  void UpdateStart();
  void UpdateEnd();

private:
  using TimePoint =
      std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds>;

  uint32_t m_numUpdates = 0;
  uint64_t m_totalTimeInUpdatesMs = 0;
  uint32_t m_minTimeInUpdatesMs = std::numeric_limits<uint32_t>::max();
  uint32_t m_maxTimeInUpdatesMs = 0;
  TimePoint m_startUpdateTime{};
};

} // namespace GOOM

#endif //VISUALIZATION_GOOM_STATS_LINE_STATS_H
