#include "filter_stats.h"

#include "filter_data.h"
#include "goom/goom_stats.h"
#include "goomutils/image_bitmaps.h"
#include "goomutils/strutils.h"
#include "image_displacement.h"

#include <chrono>
#include <cmath>
#include <cstdint>


namespace GOOM
{

using GOOM::UTILS::EnumToString;

void FilterStats::Reset()
{
  m_numUpdates = 0;
  m_totalTimeInUpdatesMs = 0;
  m_minTimeInUpdatesMs = std::numeric_limits<uint32_t>::max();
  m_maxTimeInUpdatesMs = 0;
  m_startUpdateTime = std::chrono::high_resolution_clock::now();

  m_numTranBuffersUpdates = 0;
  m_totalTimeInTranBuffersUpdatesMs = 0;
  m_minTimeInTranBuffersUpdatesMs = std::numeric_limits<uint32_t>::max();
  m_maxTimeInTranBuffersUpdatesMs = 0;
  m_startTranBuffersUpdateTime = std::chrono::high_resolution_clock::now();
  m_modeAtMinTimeOfTranBuffersUpdate = ZoomFilterMode::_NULL;
  m_bufferStateAtMinTimeOfTranBuffersUpdate = ZoomFilterBuffers::TranBufferState::_NULL;
  m_modeAtMaxTimeOfTranBuffersUpdate = ZoomFilterMode::_NULL;
  m_bufferStateAtMaxTimeOfTranBuffersUpdate = ZoomFilterBuffers::TranBufferState::_NULL;

  m_numChangeFilterSettings = 0;
  m_numZoomVectors = 0;
  m_numZoomVectorNoisify = 0;
  m_numZoomVectorChangeNoiseFactor = 0;
  m_numZoomVectorHypercosEffect = 0;
  m_numZoomVectorHPlaneEffect = 0;
  m_numZoomVectorVPlaneEffect = 0;
  m_numCZoom = 0;
  m_numZoomFilterFastRgb = 0;
  m_numRestartTranBuffer = 0;
  m_numResetTranBuffer = 0;
  m_numSwitchIncrNotZero = 0;
  m_numSwitchMultNotOne = 0;
  m_numZoomVectorTanEffect = 0;
  m_numZoomVectorNegativeRotate = 0;
  m_numZoomVectorPositiveRotate = 0;
  m_numTranPointsClipped = 0;
  m_numZoomVectorCoeffVitesseBelowMin = 0;
  m_numZoomVectorCoeffVitesseAboveMax = 0;
}

void FilterStats::Log(const GoomStats::LogStatsValueFunc& logVal) const
{
  const constexpr char* MODULE = "Filter";

  const auto avTimeInUpdateMs = static_cast<int32_t>(std::lround(
      m_numUpdates == 0
          ? -1.0
          : static_cast<float>(m_totalTimeInUpdatesMs) / static_cast<float>(m_numUpdates)));
  logVal(MODULE, "avTimeInUpdateMs", avTimeInUpdateMs);
  logVal(MODULE, "minTimeInUpdatesMs", m_minTimeInUpdatesMs);
  logVal(MODULE, "maxTimeInUpdatesMs", m_maxTimeInUpdatesMs);

  const auto avTimeInTranBuffersUpdateMs = static_cast<int32_t>(std::lround(
      m_numTranBuffersUpdates == 0 ? -1.0
                                   : static_cast<float>(m_totalTimeInTranBuffersUpdatesMs) /
                                         static_cast<float>(m_numTranBuffersUpdates)));
  logVal(MODULE, "avTimeInTranBuffersUpdateMs", avTimeInTranBuffersUpdateMs);
  logVal(MODULE, "minTimeInTranBuffersUpdatesMs", m_minTimeInTranBuffersUpdatesMs);
  logVal(MODULE, "maxTimeInTranBuffersUpdatesMs", m_maxTimeInTranBuffersUpdatesMs);
  logVal(MODULE, "m_modeAtMinTimeOfTranBuffersUpdate",
         EnumToString(m_modeAtMinTimeOfTranBuffersUpdate));
  logVal(MODULE, "m_modeAtMaxTimeOfTranBuffersUpdate",
         EnumToString(m_modeAtMaxTimeOfTranBuffersUpdate));
  logVal(MODULE, "m_bufferStateAtMinTimeOfTranBuffersUpdate",
         EnumToString(m_bufferStateAtMinTimeOfTranBuffersUpdate));
  logVal(MODULE, "m_bufferStateAtMaxTimeOfTranBuffersUpdate",
         EnumToString(m_bufferStateAtMaxTimeOfTranBuffersUpdate));

  logVal(MODULE, "lastJustChangedFilterSettings",
         static_cast<uint32_t>(m_lastJustChangedFilterSettings));
  logVal(MODULE, "lastGeneralSpeed", m_lastGeneralSpeed);
  logVal(MODULE, "lastPrevX", m_lastPrevX);
  logVal(MODULE, "lastPrevY", m_lastPrevY);
  logVal(MODULE, "lastTranBuffYLineStart", m_lastTranBuffYLineStart);
  logVal(MODULE, "lastTranDiffFactor", m_lastTranDiffFactor);

  if (m_lastZoomFilterSettings == nullptr)
  {
    logVal(MODULE, "lastZoomFilterData", 0U);
  }
  else
  {
    logVal(MODULE, "lastZoomFilterData->mode", EnumToString(m_lastZoomFilterSettings->mode));
    logVal(MODULE, "lastZoomFilterData->vitesse", m_lastZoomFilterSettings->vitesse.GetVitesse());
    logVal(MODULE, "lastZoomFilterData->reverseSpeed",
           static_cast<uint32_t>(m_lastZoomFilterSettings->vitesse.GetReverseVitesse()));
    logVal(MODULE, "lastZoomFilterData->relativeSpeed",
           m_lastZoomFilterSettings->vitesse.GetRelativeSpeed());
    logVal(MODULE, "lastZoomFilterData->pertedec", static_cast<uint32_t>(ZoomFilterData::pertedec));
    logVal(MODULE, "lastZoomFilterData->middleX", m_lastZoomFilterSettings->middleX);
    logVal(MODULE, "lastZoomFilterData->middleY", m_lastZoomFilterSettings->middleY);

    logVal(MODULE, "lastZoomFilterData->amuletteAmplitude",
           m_lastZoomFilterSettings->amuletAmplitude);
    logVal(MODULE, "lastZoomFilterData->crystalBallAmplitude",
           m_lastZoomFilterSettings->crystalBallAmplitude);
    logVal(MODULE, "lastZoomFilterData->crystalBallSqDistOffset",
           m_lastZoomFilterSettings->crystalBallSqDistOffset);
    logVal(MODULE, "lastZoomFilterData->hPlaneEffect", m_lastZoomFilterSettings->hPlaneEffect);
    logVal(MODULE, "lastZoomFilterData->vPlaneEffect", m_lastZoomFilterSettings->vPlaneEffect);
    logVal(MODULE, "lastZoomFilterData->hPlaneEffectAmplitude",
           m_lastZoomFilterSettings->hPlaneEffectAmplitude);
    logVal(MODULE, "lastZoomFilterData->vPlaneEffectAmplitude",
           m_lastZoomFilterSettings->vPlaneEffectAmplitude);
    logVal(MODULE, "lastZoomFilterData->hypercosEffect",
           EnumToString(m_lastZoomFilterSettings->hypercosEffect));
    logVal(MODULE, "lastZoomFilterData->hypercosReverse",
           static_cast<uint32_t>(m_lastZoomFilterSettings->hypercosReverse));
    logVal(MODULE, "lastZoomFilterData->hypercosFreqX", m_lastZoomFilterSettings->hypercosFreqX);
    logVal(MODULE, "lastZoomFilterData->hypercosFreqY", m_lastZoomFilterSettings->hypercosFreqY);
    logVal(MODULE, "lastZoomFilterData->hypercosAmplitudeX",
           m_lastZoomFilterSettings->hypercosAmplitudeX);
    logVal(MODULE, "lastZoomFilterData->hypercosAmplitudeY",
           m_lastZoomFilterSettings->hypercosAmplitudeY);
    if (m_lastZoomFilterSettings->imageDisplacement == nullptr)
    {
      logVal(MODULE, "lastZoomFilterData->imageDisplacementFilename", std::string(""));
    }
    else
    {
      logVal(MODULE, "lastZoomFilterData->imageDisplacementFilename",
             m_lastZoomFilterSettings->imageDisplacement->GetImageFilename());
    }
    logVal(MODULE, "lastZoomFilterData->imageDisplacementAmplitude",
           m_lastZoomFilterSettings->imageDisplacementAmplitude);
    logVal(MODULE, "lastZoomFilterData->imageDisplacementXColorCutoff",
           m_lastZoomFilterSettings->imageDisplacementXColorCutoff);
    logVal(MODULE, "lastZoomFilterData->imageDisplacementYColorCutoff",
           m_lastZoomFilterSettings->imageDisplacementYColorCutoff);
    logVal(MODULE, "lastZoomFilterData->scrunchAmplitude",
           m_lastZoomFilterSettings->scrunchAmplitude);
    logVal(MODULE, "lastZoomFilterData->speedwayAmplitude",
           m_lastZoomFilterSettings->speedwayAmplitude);
    logVal(MODULE, "lastZoomFilterData->waveEffectType",
           EnumToString(m_lastZoomFilterSettings->waveEffectType));
    logVal(MODULE, "lastZoomFilterData->waveFreqFactor", m_lastZoomFilterSettings->waveFreqFactor);
    logVal(MODULE, "lastZoomFilterData->waveAmplitude", m_lastZoomFilterSettings->waveAmplitude);
    logVal(MODULE, "lastZoomFilterData->yOnlyEffect",
           EnumToString(m_lastZoomFilterSettings->yOnlyEffect));
    logVal(MODULE, "lastZoomFilterData->yOnlyAmplitude", m_lastZoomFilterSettings->yOnlyAmplitude);
    logVal(MODULE, "lastZoomFilterData->yOnlyFreqFactor",
           m_lastZoomFilterSettings->yOnlyFreqFactor);
    logVal(MODULE, "lastZoomFilterData->yOnlyXFreqFactor",
           m_lastZoomFilterSettings->yOnlyXFreqFactor);

    logVal(MODULE, "lastZoomFilterData->noisify",
           static_cast<uint32_t>(m_lastZoomFilterSettings->noisify));
    logVal(MODULE, "lastZoomFilterData->noiseFactor",
           static_cast<float>(m_lastZoomFilterSettings->noiseFactor));
    logVal(MODULE, "lastZoomFilterData->blockyWavy",
           static_cast<uint32_t>(m_lastZoomFilterSettings->blockyWavy));
    logVal(MODULE, "lastZoomFilterData->rotateSpeed", m_lastZoomFilterSettings->rotateSpeed);
    logVal(MODULE, "lastZoomFilterData->tanEffect",
           static_cast<uint32_t>(m_lastZoomFilterSettings->tanEffect));
  }

  logVal(MODULE, "numResetTranBuffer", m_numResetTranBuffer);
  logVal(MODULE, "numRestartTranBuffer", m_numRestartTranBuffer);
  logVal(MODULE, "numSwitchIncrNotZero", m_numSwitchIncrNotZero);
  logVal(MODULE, "numSwitchMultNotOne", m_numSwitchMultNotOne);
  logVal(MODULE, "numTranPointsClipped", m_numTranPointsClipped);

  logVal(MODULE, "numChangeFilterSettings", m_numChangeFilterSettings);
  logVal(MODULE, "numCZoom", m_numCZoom);
  logVal(MODULE, "numZoomVectors", m_numZoomVectors);
  logVal(MODULE, "numZoomVectorNoisify", m_numZoomVectorNoisify);
  logVal(MODULE, "numZoomVectorChangeNoiseFactor", m_numZoomVectorChangeNoiseFactor);
  logVal(MODULE, "numZoomVectorHypercosEffect", m_numZoomVectorHypercosEffect);
  logVal(MODULE, "numZoomVectorHPlaneEffect", m_numZoomVectorHPlaneEffect);
  logVal(MODULE, "numZoomVectorVPlaneEffect", m_numZoomVectorVPlaneEffect);
  logVal(MODULE, "numZoomVectorNegativeRotate", m_numZoomVectorNegativeRotate);
  logVal(MODULE, "numZoomVectorPositiveRotate", m_numZoomVectorPositiveRotate);
  logVal(MODULE, "numZoomVectorTanEffect", m_numZoomVectorTanEffect);
  logVal(MODULE, "numZoomVectorCoeffVitesseBelowMin", m_numZoomVectorCoeffVitesseBelowMin);
  logVal(MODULE, "numZoomVectorCoeffVitesseAboveMax", m_numZoomVectorCoeffVitesseAboveMax);
}

void FilterStats::UpdateStart()
{
  m_startUpdateTime = std::chrono::high_resolution_clock::now();
  m_numUpdates++;
}

void FilterStats::UpdateEnd()
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

void FilterStats::UpdateTranBufferStart()
{
  m_startTranBuffersUpdateTime = std::chrono::high_resolution_clock::now();
  m_numTranBuffersUpdates++;
}

void FilterStats::UpdateTranBufferEnd(const ZoomFilterMode mode,
                                      const ZoomFilterBuffers::TranBufferState bufferState)
{
  const auto timeNow = std::chrono::high_resolution_clock::now();

  using Ms = std::chrono::milliseconds;
  const Ms diff = std::chrono::duration_cast<Ms>(timeNow - m_startTranBuffersUpdateTime);
  const auto timeInUpdateMs = static_cast<uint32_t>(diff.count());
  if (timeInUpdateMs < m_minTimeInTranBuffersUpdatesMs)
  {
    m_minTimeInTranBuffersUpdatesMs = timeInUpdateMs;
    m_modeAtMinTimeOfTranBuffersUpdate = mode;
    m_bufferStateAtMinTimeOfTranBuffersUpdate = bufferState;
  }
  else if (timeInUpdateMs > m_maxTimeInTranBuffersUpdatesMs)
  {
    m_maxTimeInTranBuffersUpdatesMs = timeInUpdateMs;
    m_modeAtMaxTimeOfTranBuffersUpdate = mode;
    m_bufferStateAtMaxTimeOfTranBuffersUpdate = bufferState;
  }
  m_totalTimeInTranBuffersUpdatesMs += timeInUpdateMs;
}

void FilterStats::DoChangeFilterSettings()
{
  m_numChangeFilterSettings++;
}

void FilterStats::DoZoomVector()
{
  m_numZoomVectors++;
}

void FilterStats::DoZoomVectorNoisify()
{
  m_numZoomVectorNoisify++;
}

void FilterStats::DoZoomVectorNoiseFactor()
{
  m_numZoomVectorChangeNoiseFactor++;
}

void FilterStats::DoZoomVectorHypercosEffect()
{
  m_numZoomVectorHypercosEffect++;
}

void FilterStats::DoZoomVectorHPlaneEffect()
{
  m_numZoomVectorHPlaneEffect++;
}

void FilterStats::DoZoomVectorVPlaneEffect()
{
  m_numZoomVectorVPlaneEffect++;
}

void FilterStats::DoCZoom()
{
  m_numCZoom++;
}

void FilterStats::DoZoomFilterFastRgb()
{
  m_numZoomFilterFastRgb++;
}

void FilterStats::DoResetTranBuffer()
{
  m_numResetTranBuffer++;
}

void FilterStats::DoRestartTranBuffer()
{
  m_numRestartTranBuffer++;
}

void FilterStats::DoSwitchMultNotOne()
{
  m_numSwitchMultNotOne++;
}

void FilterStats::DoSwitchIncrNotZero()
{
  m_numSwitchIncrNotZero++;
}

void FilterStats::DoZoomVectorTanEffect()
{
  m_numZoomVectorTanEffect++;
}

void FilterStats::DoZoomVectorNegativeRotate()
{
  m_numZoomVectorNegativeRotate++;
}

void FilterStats::DoZoomVectorPositiveRotate()
{
  m_numZoomVectorPositiveRotate++;
}

void FilterStats::DoTranPointClipped()
{
  m_numTranPointsClipped++;
}

void FilterStats::DoZoomVectorSpeedCoeffBelowMin()
{
  m_numZoomVectorCoeffVitesseBelowMin++;
}

void FilterStats::DoZoomVectorSpeedCoeffAboveMax()
{
  m_numZoomVectorCoeffVitesseAboveMax++;
}

void FilterStats::SetLastZoomFilterSettings(const ZoomFilterData& filterSettings)
{
  m_lastZoomFilterSettings = &filterSettings;
}

void FilterStats::SetLastJustChangedFilterSettings(bool val)
{
  m_lastJustChangedFilterSettings = val;
}

void FilterStats::SetLastGeneralSpeed(const float val)
{
  m_lastGeneralSpeed = val;
}

void FilterStats::SetLastPrevX(const uint32_t val)
{
  m_lastPrevX = val;
}

void FilterStats::SetLastPrevY(const uint32_t val)
{
  m_lastPrevY = val;
}

void FilterStats::SetLastTranBuffYLineStart(const uint32_t val)
{
  m_lastTranBuffYLineStart = val;
}

void FilterStats::SetLastTranDiffFactor(const int32_t val)
{
  m_lastTranDiffFactor = val;
}

} // namespace GOOM
