#include "sound_stats.h"

#include "goom/goom_config.h"
#include "goom/goom_stats.h"

#include <cmath>
#include <cstdint>

namespace GOOM
{

void SoundStats::Reset()
{
  m_numGooms = 0;
  m_numBigGooms = 0;

  m_lastGoomPower = 0.0;

  m_numVolumeUpdates = 0;
  m_minVolume = 1.0;
  m_maxVolume = 0.0;
  m_totalVolume = 0.0;
  m_minAcceleration = 1.0;
  m_maxAcceleration = 0.0;
  m_totalAcceleration = 0.0;
  m_numAccelerationUpdates = 0;
}

void SoundStats::Log(const GoomStats::LogStatsValueFunc& logVal) const
{
  const constexpr char* MODULE = "Sound";

  logVal(MODULE, "numGooms", m_numGooms);
  logVal(MODULE, "numBigGooms", m_numBigGooms);
  logVal(MODULE, "lastGoomPower", m_lastGoomPower);

  const float avVolume =
      m_numVolumeUpdates == 0
          ? -1.0
          : static_cast<float>(m_totalVolume / static_cast<double>(m_numVolumeUpdates));
  logVal(MODULE, "numVolumeUpdates", m_numVolumeUpdates);
  logVal(MODULE, "avVolume", avVolume);
  logVal(MODULE, "minVolume", m_minVolume);
  logVal(MODULE, "maxVolume", m_maxVolume);

  const float avAcceleration =
      m_numAccelerationUpdates == 0
          ? -1.0
          : static_cast<float>(m_totalAcceleration / static_cast<double>(m_numAccelerationUpdates));
  logVal(MODULE, "numAccelerationUpdates", m_numAccelerationUpdates);
  logVal(MODULE, "avAcceleration", avAcceleration);
  logVal(MODULE, "minAcceleration", m_minAcceleration);
  logVal(MODULE, "maxAcceleration", m_maxAcceleration);

  const float avSpeed =
      m_numSpeedUpdates == 0
          ? -1.0
          : static_cast<float>(m_totalSpeed / static_cast<double>(m_numSpeedUpdates));
  logVal(MODULE, "numSpeedUpdates", m_numSpeedUpdates);
  logVal(MODULE, "avSpeed", avSpeed);
  logVal(MODULE, "minSpeed", m_minSpeed);
  logVal(MODULE, "maxSpeed", m_maxSpeed);
}

void SoundStats::LastGoomPower(float val)
{
  m_lastGoomPower = val;
}

void SoundStats::DoGoom()
{
  m_numGooms++;
}

void SoundStats::DoBigGoom()
{
  m_numBigGooms++;
}

void SoundStats::DoUpdateVolume(const float volume)
{
  m_numVolumeUpdates++;
  m_totalVolume += volume;
  if (volume > m_maxVolume)
  {
    m_maxVolume = volume;
  }
  if (volume < m_minVolume)
  {
    m_minVolume = volume;
  }
}

void SoundStats::DoUpdateAcceleration(const float acceleration)
{
  m_numAccelerationUpdates++;
  m_totalAcceleration += acceleration;
  if (acceleration > m_maxAcceleration)
  {
    m_maxAcceleration = acceleration;
  }
  if (acceleration < m_minAcceleration)
  {
    m_minAcceleration = acceleration;
  }
}

void SoundStats::DoUpdateSpeed(float speed)
{
  m_numSpeedUpdates++;
  m_totalSpeed += speed;
  if (speed > m_maxSpeed)
  {
    m_maxSpeed = speed;
  }
  if (speed < m_minSpeed)
  {
    m_minSpeed = speed;
  }
}

} // namespace GOOM
