#include "goom_control_stats.h"

#include "goom/filters.h"
#include "goom/goom_stats.h"
#include "goom/goom_version.h"
#include "goomutils/logging.h"
#include "goomutils/strutils.h"

#include <array>
#include <chrono>
#include <cstdint>
#include <string>

namespace GOOM
{

using GOOM::UTILS::EnumToString;
using GOOM::UTILS::Logging;

void GoomControlStats::Reset()
{
  m_startingState = 0;
  m_startingFilterMode = ZoomFilterMode::_SIZE;
  m_startingSeed = 0;
  m_lastState = 0;
  m_lastSeed = 0;
  m_numThreadsUsed = 0;

  m_stateAtMin = 0;
  m_stateAtMax = 0;
  m_filterModeAtMin = ZoomFilterMode::_NULL;
  m_filterModeAtMax = ZoomFilterMode::_NULL;

  m_numUpdates = 0;
  m_totalTimeInUpdatesMs = 0;
  m_minTimeInUpdatesMs = std::numeric_limits<uint32_t>::max();
  m_maxTimeInUpdatesMs = 0;
  m_timeNowInUpdate = std::chrono::high_resolution_clock::now();

  m_numLockChanges = 0;

  m_numStateChangeRequests = 0;
  m_totalStateChanges = 0;
  m_totalStateDurations = 0;
  std::fill(m_numStateChanges.begin(), m_numStateChanges.end(), 0);
  std::fill(m_stateDurations.begin(), m_stateDurations.end(), 0);

  m_numChangeFilterModes = 0;
  m_numApplyChangeFilterSettings = 0;
  m_numFilterModeChanges.fill(0);

  m_numBigUpdates = 0;
  m_numBigBreaks = 0;
  m_numBigNormalUpdates = 0;
  m_numMegaLentChanges = 0;
  m_numChangeMilieu = 0;
  m_numChangeVitesse = 0;
  m_numChangeReverseSpeedOn = 0;
  m_numChangeReverseSpeedOff = 0;

  m_numDoIFS = 0;
  m_numIfsRenew = 0;
  m_numDoDots = 0;
  m_numDoLines = 0;
  m_numDoStars = 0;
  m_numDoTentacles = 0;
  m_numDisabledTentacles = 0;

  m_numReduceStopSpeed = 0;
  m_numIncreaseStopSpeed = 0;
  m_numStopLinesRequests = 0;
  m_numLowerVitesse = 0;
  m_numChangeSwitchValues = 0;
  m_numDoNoise = 0;
  m_numTurnOffNoise = 0;
  m_numChangeLineColor = 0;
  m_numSwitchLines = 0;
  m_numStopRotation = 0;
  m_numDecreaseRotation = 0;
  m_numIncreaseRotation = 0;
  m_numToggleRotation = 0;
  m_numBlockyWavyOn = 0;
  m_numBlockyWavyOff = 0;
  m_numZoomFilterAllowOverexposedOff = 0;
  m_numZoomFilterAllowOverexposedOn = 0;
  m_numTooManyClipped = 0;
}

void GoomControlStats::Log(const GoomStats::LogStatsValueFunc& logVal) const
{
  const constexpr char* MODULE = "goom_core";

  logVal(MODULE, "songTitle", m_songTitle);
  logVal(MODULE, "numThreadsUsed", m_numThreadsUsed);
  logVal(MODULE, "fontFileUsed", m_fontFileUsed);
  logVal(MODULE, "goomLib version", GetFullVersionStr());
  logVal(MODULE, "Compiler std", static_cast<uint32_t>(__cplusplus));

  logVal(MODULE, "startingState", m_startingState);
  logVal(MODULE, "startingFilterMode", EnumToString(m_startingFilterMode));
  logVal(MODULE, "startingSeed", m_startingSeed);
  logVal(MODULE, "lastState", m_lastState);
  logVal(MODULE, "lastSeed", m_lastSeed);
  logVal(MODULE, "lastNumClipped", m_lastNumClipped);
  logVal(MODULE, "lastFilterDuration", m_lastFilterDuration);

  logVal(MODULE, "numUpdates", m_numUpdates);
  const auto avTimeInUpdateMs = static_cast<int32_t>(std::lround(
      m_numUpdates == 0
          ? -1.0
          : static_cast<float>(m_totalTimeInUpdatesMs) / static_cast<float>(m_numUpdates)));
  logVal(MODULE, "avTimeInUpdateMs", avTimeInUpdateMs);
  logVal(MODULE, "minTimeInUpdatesMs", m_minTimeInUpdatesMs);
  logVal(MODULE, "stateAtMin", m_stateAtMin);
  logVal(MODULE, "filterModeAtMin", EnumToString(m_filterModeAtMin));
  logVal(MODULE, "maxTimeInUpdatesMs", m_maxTimeInUpdatesMs);
  logVal(MODULE, "stateAtMax", m_stateAtMax);
  logVal(MODULE, "filterModeAtMax", EnumToString(m_filterModeAtMax));
  logVal(MODULE, "totalStateChanges", m_totalStateChanges);
  const float avStateDuration =
      m_totalStateChanges == 0
          ? -1.0F
          : static_cast<float>(m_totalStateDurations) / static_cast<float>(m_totalStateChanges);
  logVal(MODULE, "averageStateDuration", avStateDuration);
  for (size_t i = 0; i < m_numStateChanges.size(); i++)
  {
    logVal(MODULE, "numState_" + std::to_string(i) + "_Changes", m_numStateChanges[i]);
  }
  for (size_t i = 0; i < m_stateDurations.size(); i++)
  {
    const float avStateTime =
        m_numStateChanges[i] == 0
            ? -1.0F
            : static_cast<float>(m_stateDurations[i]) / static_cast<float>(m_numStateChanges[i]);
    logVal(MODULE, "averageState_" + std::to_string(i) + "_Duration", avStateTime);
  }
  logVal(MODULE, "numChangeFilterModes", m_numChangeFilterModes);
  logVal(MODULE, "numApplyChangeFilterSettings", m_numApplyChangeFilterSettings);
  const float avFilterDuration = m_numApplyChangeFilterSettings == 0
                                     ? -1.0F
                                     : static_cast<float>(m_totalFilterDurations) /
                                           static_cast<float>(m_numApplyChangeFilterSettings);
  logVal(MODULE, "averageFilterDuration", avFilterDuration);
  for (size_t i = 0; i < m_numFilterModeChanges.size(); i++)
  {
    logVal(MODULE, "numFilterMode_" + EnumToString(static_cast<ZoomFilterMode>(i)) + "_Changes",
           m_numFilterModeChanges[i]);
  }

  logVal(MODULE, "numLockChanges", m_numLockChanges);
  logVal(MODULE, "numLastTimeGoomChanges", m_numBigNormalUpdates);
  logVal(MODULE, "numBigUpdates", m_numBigUpdates);
  logVal(MODULE, "numBigBreaks", m_numBigBreaks);
  logVal(MODULE, "numMegaLentChanges", m_numMegaLentChanges);
  logVal(MODULE, "numChangeMilieu", m_numChangeMilieu);
  logVal(MODULE, "numChangeVitesse", m_numChangeVitesse);
  logVal(MODULE, "numChangeReverseSpeedOff", m_numChangeReverseSpeedOff);
  logVal(MODULE, "numChangeReverseSpeedOn", m_numChangeReverseSpeedOn);

  logVal(MODULE, "numReduceStopSpeed", m_numReduceStopSpeed);
  logVal(MODULE, "numIncreaseStopSpeed", m_numIncreaseStopSpeed);
  logVal(MODULE, "numStopLinesRequests", m_numStopLinesRequests);
  logVal(MODULE, "numLowerVitesse", m_numLowerVitesse);
  logVal(MODULE, "numChangeSwitchValues", m_numChangeSwitchValues);
  logVal(MODULE, "numDoNoise", m_numDoNoise);
  logVal(MODULE, "numTurnOffNoise", m_numTurnOffNoise);
  logVal(MODULE, "numChangeLineColor", m_numChangeLineColor);
  logVal(MODULE, "numSwitchLines", m_numSwitchLines);
  logVal(MODULE, "numStopRotation", m_numStopRotation);
  logVal(MODULE, "numDecreaseRotation", m_numDecreaseRotation);
  logVal(MODULE, "numIncreaseRotation", m_numIncreaseRotation);
  logVal(MODULE, "numToggleRotation", m_numToggleRotation);
  logVal(MODULE, "numBlockyWavyOff", m_numBlockyWavyOff);
  logVal(MODULE, "numBlockyWavyOn", m_numBlockyWavyOn);
  logVal(MODULE, "numZoomFilterAllowOverexposedOff", m_numZoomFilterAllowOverexposedOff);
  logVal(MODULE, "numZoomFilterAllowOverexposedOn", m_numZoomFilterAllowOverexposedOn);
  logVal(MODULE, "numTooManyClipped", m_numTooManyClipped);

  logVal(MODULE, "numDoIFS", m_numDoIFS);
  logVal(MODULE, "numIfsRenew", m_numIfsRenew);
  logVal(MODULE, "numDoDots", m_numDoDots);
  logVal(MODULE, "numDoLines", m_numDoLines);
  logVal(MODULE, "numDoStars", m_numDoStars);
  logVal(MODULE, "numDoTentacles", m_numDoTentacles);
  logVal(MODULE, "numDisabledTentacles", m_numDisabledTentacles);
}

void GoomControlStats::SetSongTitle(const std::string& s)
{
  m_songTitle = s;
}

void GoomControlStats::SetStateStartValue(const uint32_t stateIndex)
{
  m_startingState = stateIndex;
}

void GoomControlStats::SetZoomFilterStartValue(const ZoomFilterMode filterMode)
{
  m_startingFilterMode = filterMode;
}

void GoomControlStats::SetStateLastValue(const uint32_t stateIndex)
{
  m_lastState = stateIndex;
}

void GoomControlStats::SetSeedStartValue(const uint64_t seed)
{
  m_startingSeed = seed;
}

void GoomControlStats::SetSeedLastValue(const uint64_t seed)
{
  m_lastSeed = seed;
}

void GoomControlStats::SetNumThreadsUsedValue(const size_t n)
{
  m_numThreadsUsed = n;
}

void GoomControlStats::SetFontFileUsed(const std::string& f)
{
  m_fontFileUsed = f;
}

void GoomControlStats::UpdateChange(const size_t currentState,
                                    const ZoomFilterMode currentFilterMode)
{
  const auto timeNow = std::chrono::high_resolution_clock::now();
  if (m_numUpdates > 0)
  {
    using Ms = std::chrono::milliseconds;
    const Ms diff = std::chrono::duration_cast<Ms>(timeNow - m_timeNowInUpdate);
    const auto timeInUpdateMs = static_cast<uint32_t>(diff.count());
    if (timeInUpdateMs < m_minTimeInUpdatesMs)
    {
      m_minTimeInUpdatesMs = timeInUpdateMs;
      m_stateAtMin = currentState;
      m_filterModeAtMin = currentFilterMode;
    }
    else if (timeInUpdateMs > m_maxTimeInUpdatesMs)
    {
      m_maxTimeInUpdatesMs = timeInUpdateMs;
      m_stateAtMax = currentState;
      m_filterModeAtMax = currentFilterMode;
    }
    m_totalTimeInUpdatesMs += timeInUpdateMs;
  }
  m_timeNowInUpdate = timeNow;

  m_numUpdates++;
}

void GoomControlStats::DoStateChangeRequest()
{
  m_numStateChangeRequests++;
}

void GoomControlStats::DoStateChange(const uint32_t timeInState)
{
  m_totalStateChanges++;
  m_totalStateDurations += timeInState;
}

void GoomControlStats::DoStateChange(const size_t index, const uint32_t timeInState)
{
  if (index >= m_numStateChanges.size())
  {
    for (size_t i = m_numStateChanges.size(); i <= index; i++)
    {
      m_numStateChanges.push_back(0);
    }
  }
  m_numStateChanges[index]++;

  if (index >= m_stateDurations.size())
  {
    for (size_t i = m_stateDurations.size(); i <= index; i++)
    {
      m_stateDurations.push_back(0);
    }
  }
  m_stateDurations[index] += timeInState;
}

void GoomControlStats::DoChangeFilterMode()
{
  m_numChangeFilterModes++;
}

void GoomControlStats::DoApplyChangeFilterSettings(uint32_t timeWithFilter)
{
  m_numApplyChangeFilterSettings++;
  m_totalFilterDurations += timeWithFilter;
  m_lastFilterDuration = timeWithFilter;
}

void GoomControlStats::DoChangeFilterMode(const ZoomFilterMode mode)
{
  m_numFilterModeChanges.at(static_cast<size_t>(mode))++;
}

void GoomControlStats::DoLockChange()
{
  m_numLockChanges++;
}

void GoomControlStats::DoIfs()
{
  m_numDoIFS++;
}

void GoomControlStats::DoDots()
{
  m_numDoDots++;
}

void GoomControlStats::DoLines()
{
  m_numDoLines++;
}

void GoomControlStats::DoStars()
{
  m_numDoStars++;
}

void GoomControlStats::DoTentacles()
{
  m_numDoTentacles++;
}

void GoomControlStats::DoBigBreak()
{
  m_numBigBreaks++;
}

void GoomControlStats::DoBigUpdate()
{
  m_numBigUpdates++;
}

void GoomControlStats::DoBigNormalUpdate()
{
  m_numBigNormalUpdates++;
}

void GoomControlStats::DoMegaLentChange()
{
  m_numMegaLentChanges++;
}

void GoomControlStats::DoChangeMilieu()
{
  m_numChangeMilieu++;
}

void GoomControlStats::DoChangeVitesse()
{
  m_numChangeVitesse++;
}

void GoomControlStats::DoChangeReverseSpeedOff()
{
  m_numChangeReverseSpeedOff++;
}

void GoomControlStats::DoChangeReverseSpeedOn()
{
  m_numChangeReverseSpeedOn++;
}

void GoomControlStats::DoReduceStopSpeed()
{
  m_numReduceStopSpeed++;
}

void GoomControlStats::DoIncreaseStopSpeed()
{
  m_numIncreaseStopSpeed++;
}

void GoomControlStats::DoStopLinesRequest()
{
  m_numStopLinesRequests++;
}

void GoomControlStats::DoLowerVitesse()
{
  m_numLowerVitesse++;
}

void GoomControlStats::DoChangeSwitchValues()
{
  m_numChangeSwitchValues++;
}

void GoomControlStats::DoNoise()
{
  m_numDoNoise++;
}

void GoomControlStats::DoTurnOffNoise()
{
  m_numTurnOffNoise++;
}

void GoomControlStats::DoIfsRenew()
{
  m_numIfsRenew++;
}

void GoomControlStats::DoChangeLineColor()
{
  m_numChangeLineColor++;
}

void GoomControlStats::DoStopRotation()
{
  m_numStopRotation++;
}

void GoomControlStats::DoDecreaseRotation()
{
  m_numDecreaseRotation++;
}

void GoomControlStats::DoIncreaseRotation()
{
  m_numIncreaseRotation++;
}

void GoomControlStats::DoToggleRotation()
{
  m_numToggleRotation++;
}

void GoomControlStats::DoSwitchLines()
{
  m_numSwitchLines++;
}

void GoomControlStats::DoBlockyWavyOff()
{
  m_numBlockyWavyOff++;
}

void GoomControlStats::DoBlockyWavyOn()
{
  m_numBlockyWavyOn++;
}

void GoomControlStats::DoZoomFilterAllowOverexposedOff()
{
  m_numZoomFilterAllowOverexposedOff++;
}

void GoomControlStats::DoZoomFilterAllowOverexposedOn()
{
  m_numZoomFilterAllowOverexposedOn++;
}

void GoomControlStats::TooManyClipped()
{
  m_numTooManyClipped++;
}

void GoomControlStats::SetLastNumClipped(const uint32_t val)
{
  m_lastNumClipped = val;
}

} // namespace GOOM
