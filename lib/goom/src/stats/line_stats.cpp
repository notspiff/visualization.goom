#include "line_stats.h"

#include "goom/goom_config.h"
#include "goomutils/strutils.h"

#include <chrono>
#include <cmath>
#include <cstdint>


namespace GOOM
{

using GOOM::UTILS::EnumToString;

void LineStats::Reset()
{
  m_numUpdates = 0;
  m_totalTimeInUpdatesMs = 0;
  m_minTimeInUpdatesMs = std::numeric_limits<uint32_t>::max();
  m_maxTimeInUpdatesMs = 0;
  m_startUpdateTime = std::chrono::high_resolution_clock::now();
}

void LineStats::Log(const GoomStats::LogStatsValueFunc& logVal) const
{
  const constexpr char* MODULE = "Line";

  const auto avTimeInUpdateMs = static_cast<int32_t>(std::lround(
      m_numUpdates == 0
          ? -1.0
          : static_cast<float>(m_totalTimeInUpdatesMs) / static_cast<float>(m_numUpdates)));
  logVal(MODULE, "avTimeInUpdateMs", avTimeInUpdateMs);
  logVal(MODULE, "minTimeInUpdatesMs", m_minTimeInUpdatesMs);
  logVal(MODULE, "maxTimeInUpdatesMs", m_maxTimeInUpdatesMs);
}

void LineStats::UpdateStart()
{
  m_startUpdateTime = std::chrono::high_resolution_clock::now();
  m_numUpdates++;
}

void LineStats::UpdateEnd()
{
  const auto timeNow = std::chrono::high_resolution_clock::now();

  using Ms = std::chrono::milliseconds;
  const Ms diff = std::chrono::duration_cast<Ms>(timeNow - m_startUpdateTime);
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

} // namespace GOOM
