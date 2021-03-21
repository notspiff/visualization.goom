#ifndef VISUALIZATION_GOOM_STATS_FILTER_STATS_H
#define VISUALIZATION_GOOM_STATS_FILTER_STATS_H

#include "filter_data.h"
#include "goom/filter_buffers.h"
#include "goom/goom_config.h"
#include "goom/goom_stats.h"

#include <chrono>
#include <cstdint>
#include <string>

namespace GOOM
{

class FilterStats
{
public:
  FilterStats() noexcept = default;

  void Reset();
  void Log(const GoomStats::LogStatsValueFunc& l) const;

  void SetLastZoomFilterSettings(const ZoomFilterData& filterSettings);
  void SetLastJustChangedFilterSettings(bool val);
  void SetLastGeneralSpeed(float val);
  void SetLastPrevX(uint32_t val);
  void SetLastPrevY(uint32_t val);
  void SetLastTranBuffYLineStart(uint32_t val);
  void SetLastTranDiffFactor(int val);

  void UpdateStart();
  void UpdateEnd();

  void UpdateTranBufferStart();
  void UpdateTranBufferEnd(ZoomFilterMode mode, ZoomFilterBuffers::TranBufferState bufferState);

  void DoChangeFilterSettings();
  void DoZoomFilterFastRgb();
  void DoCZoom();
  void DoResetTranBuffer();
  void DoRestartTranBuffer();
  void DoSwitchIncrNotZero();
  void DoSwitchMultNotOne();
  void DoTranPointClipped();
  void DoZoomVector();
  void DoZoomVectorNoisify();
  void DoZoomVectorNoiseFactor();
  void DoZoomVectorHypercosEffect();
  void DoZoomVectorHPlaneEffect();
  void DoZoomVectorVPlaneEffect();
  void DoZoomVectorTanEffect();
  void DoZoomVectorNegativeRotate();
  void DoZoomVectorPositiveRotate();
  void DoZoomVectorSpeedCoeffBelowMin();
  void DoZoomVectorSpeedCoeffAboveMax();

private:
  using TimePoint =
      std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds>;

  uint32_t m_numUpdates = 0;
  uint64_t m_totalTimeInUpdatesMs = 0;
  uint32_t m_minTimeInUpdatesMs = std::numeric_limits<uint32_t>::max();
  uint32_t m_maxTimeInUpdatesMs = 0;
  TimePoint m_startUpdateTime{};

  uint32_t m_numTranBuffersUpdates = 0;
  uint64_t m_totalTimeInTranBuffersUpdatesMs = 0;
  uint32_t m_minTimeInTranBuffersUpdatesMs = std::numeric_limits<uint32_t>::max();
  uint32_t m_maxTimeInTranBuffersUpdatesMs = 0;
  TimePoint m_startTranBuffersUpdateTime{};
  ZoomFilterMode m_modeAtMinTimeOfTranBuffersUpdate{ZoomFilterMode::_NULL};
  ZoomFilterBuffers::TranBufferState m_bufferStateAtMinTimeOfTranBuffersUpdate{
      ZoomFilterBuffers::TranBufferState::_NULL};
  ZoomFilterMode m_modeAtMaxTimeOfTranBuffersUpdate{ZoomFilterMode::_NULL};
  ZoomFilterBuffers::TranBufferState m_bufferStateAtMaxTimeOfTranBuffersUpdate{
      ZoomFilterBuffers::TranBufferState::_NULL};

  bool m_lastJustChangedFilterSettings = false;
  float m_lastGeneralSpeed = -1000.0;
  uint32_t m_lastPrevX = 0;
  uint32_t m_lastPrevY = 0;
  uint32_t m_lastTranBuffYLineStart = +1000000;
  int32_t m_lastTranDiffFactor = -1000;
  const ZoomFilterData* m_lastZoomFilterSettings{};

  uint32_t m_numChangeFilterSettings = 0;
  uint64_t m_numZoomFilterFastRgb = 0;
  uint64_t m_numRestartTranBuffer = 0;
  uint64_t m_numResetTranBuffer = 0;
  uint64_t m_numSwitchIncrNotZero = 0;
  uint64_t m_numSwitchMultNotOne = 0;
  uint64_t m_numZoomVectorTanEffect = 0;
  uint64_t m_numZoomVectorNegativeRotate = 0;
  uint64_t m_numZoomVectorPositiveRotate = 0;
  uint64_t m_numTranPointsClipped = 0;
  uint64_t m_numZoomVectors = 0;
  uint64_t m_numZoomVectorNoisify = 0;
  uint64_t m_numZoomVectorChangeNoiseFactor = 0;
  uint64_t m_numZoomVectorHypercosEffect = 0;
  uint64_t m_numZoomVectorHPlaneEffect = 0;
  uint64_t m_numZoomVectorVPlaneEffect = 0;
  uint64_t m_numCZoom = 0;
  uint64_t m_numZoomVectorCoeffVitesseBelowMin = 0;
  uint64_t m_numZoomVectorCoeffVitesseAboveMax = 0;
};

} // namespace GOOM

#endif //VISUALIZATION_GOOM_STATS_FILTER_STATS_H
