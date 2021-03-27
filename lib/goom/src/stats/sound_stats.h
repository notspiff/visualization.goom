#ifndef VISUALIZATION_GOOM_STATS_SOUND_STATS_H
#define VISUALIZATION_GOOM_STATS_SOUND_STATS_H

#include "goom/goom_config.h"
#include "goom/goom_stats.h"

#include <cstdint>

namespace GOOM
{

class SoundStats
{
public:
  SoundStats() = default;

  void Reset();
  void Log(const GoomStats::LogStatsValueFunc& logVal) const;

  void LastGoomPower(float val);

  void DoGoom();
  void DoBigGoom();
  void DoUpdateVolume(float volume);
  void DoUpdateAcceleration(float acceleration);
  void DoUpdateSpeed(float speed);

private:
  uint64_t m_numGooms = 0;
  uint64_t m_numBigGooms = 0;

  float m_lastGoomPower = 0.0;

  float m_minVolume = 1.0;
  float m_maxVolume = 0.0;
  double m_totalVolume = 0.0;
  uint64_t m_numVolumeUpdates = 0;

  float m_minAcceleration = 1.0;
  float m_maxAcceleration = 0.0;
  double m_totalAcceleration = 0.0;
  uint64_t m_numAccelerationUpdates = 0;

  float m_minSpeed = 1.0;
  float m_maxSpeed = 0.0;
  double m_totalSpeed = 0.0;
  uint64_t m_numSpeedUpdates = 0;
};

} // namespace GOOM

#endif //VISUALIZATION_GOOM_STATS_SOUND_STATS_H
