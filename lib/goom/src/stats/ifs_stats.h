#ifndef VISUALIZATION_GOOM_STATS_IFS_STATS_H
#define VISUALIZATION_GOOM_STATS_IFS_STATS_H

#include "goom/goom_config.h"
#include "goom/goom_stats.h"

#include <chrono>
#include <cstdint>

namespace GOOM
{

class IfsStats
{
public:
  IfsStats() = default;

  void Reset();
  void Log(const GoomStats::LogStatsValueFunc& l) const;
  void UpdateStart();
  void UpdateEnd();

  void UpdateIfsIncr(int val);
  void SetlastIfsIncr(int val);

  void UpdateCycleChanges();
  void UpdateStdIfsFunc();
  void UpdateReverseIfsFunc();
  void UpdateLowLowDensityBlurThreshold();
  void UpdateHighLowDensityBlurThreshold();

private:
  uint32_t m_numUpdates = 0;
  uint64_t m_totalTimeInUpdatesMs = 0;
  uint32_t m_minTimeInUpdatesMs = std::numeric_limits<uint32_t>::max();
  uint32_t m_maxTimeInUpdatesMs = 0;
  std::chrono::high_resolution_clock::time_point m_timeNowHiRes{};
  int m_maxIfsIncr = -1000;
  int m_lastIfsIncr = 0;
  uint32_t m_numCycleChanges = 0;
  uint32_t m_numStdIfsFunc = 0;
  uint32_t m_numReverseIfsFunc = 0;
  uint32_t m_numLowLowDensityBlurThreshold = 0;
  uint32_t m_numHighLowDensityBlurThreshold = 0;
};

} // namespace GOOM

#endif //VISUALIZATION_GOOM_STATS_IFS_STATS_H
