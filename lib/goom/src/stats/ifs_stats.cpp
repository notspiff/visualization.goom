#include "ifs_stats.h"

#include "goom/goom_config.h"
#include "goom/goom_stats.h"

#include <chrono>
#include <cmath>
#include <cstdint>

namespace GOOM
{

void IfsStats::Reset()
{
  m_numUpdates = 0;
  m_totalTimeInUpdatesMs = 0;
  m_minTimeInUpdatesMs = std::numeric_limits<uint32_t>::max();
  m_maxTimeInUpdatesMs = 0;
  m_timeNowHiRes = std::chrono::high_resolution_clock::now();
  m_maxIfsIncr = -1000;
  m_lastIfsIncr = 0;
  m_numCycleChanges = 0;
  m_numStdIfsFunc = 0;
  m_numReverseIfsFunc = 0;
  m_numLowLowDensityBlurThreshold = 0;
  m_numHighLowDensityBlurThreshold = 0;
}

void IfsStats::Log(const GoomStats::LogStatsValueFunc& l) const
{
  const constexpr char* MODULE = "Ifs";

  const auto avTimeInUpdateMs = static_cast<int32_t>(std::lround(
      m_numUpdates == 0
          ? -1.0
          : static_cast<float>(m_totalTimeInUpdatesMs) / static_cast<float>(m_numUpdates)));
  l(MODULE, "avTimeInUpdateMs", avTimeInUpdateMs);
  l(MODULE, "minTimeInUpdatesMs", m_minTimeInUpdatesMs);
  l(MODULE, "maxTimeInUpdatesMs", m_maxTimeInUpdatesMs);
  l(MODULE, "maxIfsIncr", m_maxIfsIncr);
  l(MODULE, "lastIfsIncr", m_lastIfsIncr);
  l(MODULE, "numCycleChanges", m_numCycleChanges);
  l(MODULE, "numStdIfsFunc", m_numStdIfsFunc);
  l(MODULE, "numReverseIfsFunc", m_numReverseIfsFunc);
  l(MODULE, "numLowLowDensityBlurThreshold", m_numLowLowDensityBlurThreshold);
  l(MODULE, "numHighLowDensityBlurThreshold", m_numHighLowDensityBlurThreshold);
}

void IfsStats::UpdateStart()
{
  m_timeNowHiRes = std::chrono::high_resolution_clock::now();
  m_numUpdates++;
}

void IfsStats::UpdateEnd()
{
  const auto timeNow = std::chrono::high_resolution_clock::now();

  using Ms = std::chrono::milliseconds;
  const Ms diff = std::chrono::duration_cast<Ms>(timeNow - m_timeNowHiRes);
  const auto timeInUpdateMs = static_cast<uint32_t>(diff.count());
  if (timeInUpdateMs < m_minTimeInUpdatesMs)
  {
    m_minTimeInUpdatesMs = timeInUpdateMs;
  }
  else if (timeInUpdateMs > m_maxTimeInUpdatesMs)
  {
    m_maxTimeInUpdatesMs = timeInUpdateMs;
  }
  m_totalTimeInUpdatesMs += timeInUpdateMs;
}

void IfsStats::UpdateIfsIncr(int val)
{
  if (val > m_maxIfsIncr)
  {
    m_maxIfsIncr = val;
  }
}

void IfsStats::SetlastIfsIncr(int val)
{
  m_lastIfsIncr = val;
}

void IfsStats::UpdateCycleChanges()
{
  m_numCycleChanges++;
}

void IfsStats::UpdateStdIfsFunc()
{
  m_numStdIfsFunc++;
}

void IfsStats::UpdateReverseIfsFunc()
{
  m_numReverseIfsFunc++;
}

void IfsStats::UpdateLowLowDensityBlurThreshold()
{
  m_numLowLowDensityBlurThreshold++;
}

void IfsStats::UpdateHighLowDensityBlurThreshold()
{
  m_numHighLowDensityBlurThreshold++;
}

} // namespace GOOM
