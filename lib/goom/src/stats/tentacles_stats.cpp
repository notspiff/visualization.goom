#include "tentacles_stats.h"

#include "goom/goom_config.h"
#include "goom/goom_stats.h"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace GOOM
{

void TentacleStats::Reset()
{
  m_totalTimeInUpdatesMs = 0;
  m_minTimeInUpdatesMs = std::numeric_limits<uint32_t>::max();
  m_maxTimeInUpdatesMs = 0;
  m_timeNowHiRes = std::chrono::high_resolution_clock::now();

  m_numDominantColorMapChanges = 0;
  m_numDominantColorChanges = 0;
  m_numUpdatesWithDraw = 0;
  m_numUpdatesWithNoDraw = 0;
  m_numUpdatesWithPrettyMoveNoDraw = 0;
  m_numUpdatesWithPrettyMoveDraw = 0;
  m_numLowToMediumAcceleration = 0;
  m_numHighAcceleration = 0;
  m_numCycleResets = 0;
  m_numPrettyMoveHappens = 0;
  std::fill(m_numDriverChanges.begin(), m_numDriverChanges.end(), 0);
}

void TentacleStats::Log(const GoomStats::LogStatsValueFunc& logVal) const
{
  const constexpr char* MODULE = "Tentacles";

  const uint32_t numUpdates = m_numUpdatesWithDraw;
  const auto avTimeInUpdateMs = static_cast<int32_t>(std::lround(
      numUpdates == 0
          ? -1.0
          : static_cast<float>(m_totalTimeInUpdatesMs) / static_cast<float>(numUpdates)));
  logVal(MODULE, "avTimeInUpdateMs", avTimeInUpdateMs);
  logVal(MODULE, "minTimeInUpdatesMs", m_minTimeInUpdatesMs);
  logVal(MODULE, "maxTimeInUpdatesMs", m_maxTimeInUpdatesMs);

  logVal(MODULE, "lastNumTentacles", m_lastNumTentacles);
  logVal(MODULE, "lastUpdatingWithDraw", m_lastUpdatingWithDraw);
  logVal(MODULE, "lastCycle", m_lastCycle);
  logVal(MODULE, "lastCycleInc", m_lastCycleInc);
  logVal(MODULE, "lastLig", m_lastLig);
  logVal(MODULE, "lastLigs", m_lastLigs);
  logVal(MODULE, "lastDistt", m_lastDistt);
  logVal(MODULE, "lastDistt2", m_lastDistt2);
  logVal(MODULE, "lastDistt2Offset", m_lastDistt2Offset);
  logVal(MODULE, "lastRot", m_lastRot);
  logVal(MODULE, "lastRotAtStartOfPrettyMove", m_lastRotAtStartOfPrettyMove);
  logVal(MODULE, "lastDoRotation", m_lastDoRotation);
  logVal(MODULE, "lastIsPrettyMoveHappening", m_lastIsPrettyMoveHappening);
  logVal(MODULE, "lastPrettyMoveHappeningTimer", m_lastPrettyMoveHappeningTimer);
  logVal(MODULE, "lastPrettyMoveCheckStopMark", m_lastPrettyMoveCheckStopMark);
  logVal(MODULE, "lastPrePrettyMoveLock", m_lastPrePrettyMoveLock);
  logVal(MODULE, "lastDistt2OffsetPreStep", m_lastDistt2OffsetPreStep);
  logVal(MODULE, "lastPrettyMoveReadyToStart", m_lastPrettyMoveReadyToStart);
  logVal(MODULE, "lastPostPrettyMoveLock", m_lastPostPrettyMoveLock);
  logVal(MODULE, "lastPrettyLerpMixValue", m_lastPrettyLerpMixValue);

  logVal(MODULE, "numDominantColorMapChanges", m_numDominantColorMapChanges);
  logVal(MODULE, "numDominantColorChanges", m_numDominantColorChanges);
  logVal(MODULE, "numUpdates", numUpdates);
  logVal(MODULE, "numUpdatesWithDraw", m_numUpdatesWithDraw);
  logVal(MODULE, "numUpdatesWithNoDraw", m_numUpdatesWithNoDraw);
  logVal(MODULE, "numUpdatesWithPrettyMoveNoDraw", m_numUpdatesWithPrettyMoveNoDraw);
  logVal(MODULE, "numUpdatesWithPrettyMoveDraw", m_numUpdatesWithPrettyMoveDraw);
  logVal(MODULE, "numLowToMediumAcceleration", m_numLowToMediumAcceleration);
  logVal(MODULE, "numHighAcceleration", m_numHighAcceleration);
  logVal(MODULE, "numCycleResets", m_numCycleResets);
  logVal(MODULE, "numPrettyMoveHappens", m_numPrettyMoveHappens);
  logVal(MODULE, "numTentacleDrivers", m_numTentacleDrivers);
  // TODO Make this a string util function
  std::string numTentaclesStr;
  std::string numDriverChangesStr;
  for (size_t i = 0; i < m_numDriverTentacles.size(); i++)
  {
    if (i > 0)
    {
      numTentaclesStr += ", ";
      numDriverChangesStr += ", ";
    }
    numTentaclesStr += std::to_string(m_numDriverTentacles[i]);
    numDriverChangesStr += std::to_string(m_numDriverChanges[i]);
  }
  logVal(MODULE, "numDriverTentacles", numTentaclesStr);
  logVal(MODULE, "numDriverChangesStr", numDriverChangesStr);
}

void TentacleStats::UpdateStart()
{
  m_timeNowHiRes = std::chrono::high_resolution_clock::now();
}

void TentacleStats::UpdateEnd()
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

void TentacleStats::ChangeDominantColorMap()
{
  m_numDominantColorMapChanges++;
}

void TentacleStats::ChangeDominantColor()
{
  m_numDominantColorChanges++;
}

void TentacleStats::UpdateWithDraw()
{
  m_numUpdatesWithDraw++;
}


void TentacleStats::UpdateWithNoDraw()
{
  m_numUpdatesWithNoDraw++;
}

void TentacleStats::UpdateWithPrettyMoveNoDraw()
{
  m_numUpdatesWithPrettyMoveNoDraw++;
}

void TentacleStats::UpdateWithPrettyMoveDraw()
{
  m_numUpdatesWithPrettyMoveDraw++;
}

void TentacleStats::LowToMediumAcceleration()
{
  m_numLowToMediumAcceleration++;
}

void TentacleStats::HighAcceleration()
{
  m_numHighAcceleration++;
}

void TentacleStats::CycleReset()
{
  m_numCycleResets++;
}

void TentacleStats::PrettyMoveHappens()
{
  m_numPrettyMoveHappens++;
}

void TentacleStats::SetLastNumTentacles(const size_t val)
{
  m_lastNumTentacles = val;
}

void TentacleStats::SetLastUpdatingWithDraw(const bool val)
{
  m_lastUpdatingWithDraw = val;
}

void TentacleStats::SetLastCycle(const float val)
{
  m_lastCycle = val;
}

void TentacleStats::SetLastCycleInc(const float val)
{
  m_lastCycleInc = val;
}

void TentacleStats::SetLastLig(const float val)
{
  m_lastLig = val;
}

void TentacleStats::SetLastLigs(const float val)
{
  m_lastLigs = val;
}

void TentacleStats::SetLastDistt(const float val)
{
  m_lastDistt = val;
}

void TentacleStats::SetLastDistt2(const float val)
{
  m_lastDistt2 = val;
}

void TentacleStats::SetLastDistt2Offset(const float val)
{
  m_lastDistt2Offset = val;
}

void TentacleStats::SetLastRot(const float val)
{
  m_lastRot = val;
}

void TentacleStats::SetLastRotAtStartOfPrettyMove(const float val)
{
  m_lastRotAtStartOfPrettyMove = val;
}

void TentacleStats::SetLastDoRotation(const bool val)
{
  m_lastDoRotation = val;
}

void TentacleStats::SetLastIsPrettyMoveHappening(const bool val)
{
  m_lastIsPrettyMoveHappening = val;
}

void TentacleStats::SetLastPrettyMoveHappeningTimer(const int32_t val)
{
  m_lastPrettyMoveHappeningTimer = val;
}

void TentacleStats::SetLastPrettyMoveCheckStopMark(const int32_t val)
{
  m_lastPrettyMoveCheckStopMark = val;
}

void TentacleStats::SetLastPrePrettyMoveLock(const int32_t val)
{
  m_lastPrePrettyMoveLock = val;
}

void TentacleStats::SetLastDistt2OffsetPreStep(const float val)
{
  m_lastDistt2OffsetPreStep = val;
}

void TentacleStats::SetLastPrettyMoveReadyToStart(const bool val)
{
  m_lastPrettyMoveReadyToStart = val;
}

void TentacleStats::SetLastPostPrettyMoveLock(const int32_t val)
{
  m_lastPostPrettyMoveLock = val;
}

void TentacleStats::SetLastPrettyLerpMixValue(const float val)
{
  m_lastPrettyLerpMixValue = val;
}

void TentacleStats::SetNumTentacleDrivers(
    const std::vector<std::unique_ptr<GOOM::TentacleDriver>>& d)
{
  m_numTentacleDrivers = d.size();
  m_numDriverTentacles.resize(m_numTentacleDrivers);
  m_numDriverChanges.resize(m_numTentacleDrivers);
  for (size_t i = 0; i < m_numTentacleDrivers; i++)
  {
    m_numDriverTentacles[i] = d[i]->GetNumTentacles();
    m_numDriverChanges[i] = 0;
  }
}

void TentacleStats::ChangeTentacleDriver(const uint32_t driverIndex)
{
  m_numDriverChanges.at(driverIndex)++;
}

} // namespace GOOM
