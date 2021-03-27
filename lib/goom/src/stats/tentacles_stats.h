#ifndef VISUALIZATION_GOOM_STATS_TENTACLES_STATS_H
#define VISUALIZATION_GOOM_STATS_TENTACLES_STATS_H

#include "goom/goom_config.h"
#include "goom/goom_stats.h"
#include "goom/tentacle_driver.h"

#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>

namespace GOOM
{

class TentacleStats
{
public:
  TentacleStats() noexcept = default;

  void Reset();
  void Log(const GoomStats::LogStatsValueFunc& l) const;
  void UpdateStart();
  void UpdateEnd();
  void ChangeDominantColorMap();
  void ChangeDominantColor();
  void UpdateWithDraw();
  void UpdateWithNoDraw();
  void UpdateWithPrettyMoveNoDraw();
  void UpdateWithPrettyMoveDraw();
  void LowToMediumAcceleration();
  void HighAcceleration();
  void CycleReset();
  void PrettyMoveHappens();
  void ChangePrettyLerpMixLower();
  void ChangePrettyLerpMixHigher();
  void SetNumTentacleDrivers(const std::vector<std::unique_ptr<GOOM::TentacleDriver>>& d);
  void ChangeTentacleDriver(uint32_t driverIndex);

  void SetLastNumTentacles(size_t val);
  void SetLastUpdatingWithDraw(bool val);
  void SetLastCycle(float val);
  void SetLastCycleInc(float val);
  void SetLastLig(float val);
  void SetLastLigs(float val);
  void SetLastDistt(float val);
  void SetLastDistt2(float val);
  void SetLastDistt2Offset(float val);
  void SetLastRot(float val);
  void SetLastRotAtStartOfPrettyMove(float val);
  void SetLastDoRotation(bool val);
  void SetLastIsPrettyMoveHappening(bool val);
  void SetLastPrettyMoveHappeningTimer(int32_t val);
  void SetLastPrettyMoveCheckStopMark(int32_t val);
  void SetLastPrePrettyMoveLock(int32_t val);
  void SetLastDistt2OffsetPreStep(float val);
  void SetLastPrettyMoveReadyToStart(bool val);
  void SetLastPostPrettyMoveLock(int32_t val);
  void SetLastPrettyLerpMixValue(float val);

private:
  uint64_t m_totalTimeInUpdatesMs = 0;
  uint32_t m_minTimeInUpdatesMs = std::numeric_limits<uint32_t>::max();
  uint32_t m_maxTimeInUpdatesMs = 0;
  std::chrono::high_resolution_clock::time_point m_timeNowHiRes{};

  uint32_t m_numDominantColorMapChanges = 0;
  uint32_t m_numDominantColorChanges = 0;
  uint32_t m_numUpdatesWithDraw = 0;
  uint32_t m_numUpdatesWithNoDraw = 0;
  uint32_t m_numUpdatesWithPrettyMoveNoDraw = 0;
  uint32_t m_numUpdatesWithPrettyMoveDraw = 0;
  uint32_t m_numLowToMediumAcceleration = 0;
  uint32_t m_numHighAcceleration = 0;
  uint32_t m_numCycleResets = 0;
  uint32_t m_numPrettyMoveHappens = 0;
  uint32_t m_numTentacleDrivers = 0;
  std::vector<uint32_t> m_numDriverTentacles{};
  std::vector<uint32_t> m_numDriverChanges{};

  uint32_t m_lastNumTentacles = 0;
  bool m_lastUpdatingWithDraw = false;
  float m_lastCycle = 0.0;
  float m_lastCycleInc = 0.0;
  float m_lastLig = 0.0;
  float m_lastLigs = 0.0;
  float m_lastDistt = 0.0;
  float m_lastDistt2 = 0.0;
  float m_lastDistt2Offset = 0.0;
  float m_lastRot = 0.0;
  float m_lastRotAtStartOfPrettyMove = 0.0;
  bool m_lastDoRotation = false;
  bool m_lastIsPrettyMoveHappening = false;
  int32_t m_lastPrettyMoveHappeningTimer = 0;
  int32_t m_lastPrettyMoveCheckStopMark = 0;
  int32_t m_lastPrePrettyMoveLock = 0;
  float m_lastDistt2OffsetPreStep = 0.0;
  bool m_lastPrettyMoveReadyToStart = false;
  int32_t m_lastPostPrettyMoveLock = 0;
  float m_lastPrettyLerpMixValue = 0.0;
};

} // namespace GOOM

#endif //VISUALIZATION_GOOM_STATS_TENTACLES_STATS_H
