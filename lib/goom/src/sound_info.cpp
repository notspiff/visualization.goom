#include "sound_info.h"

#include "goom_config.h"
#include "goom_stats.h"
#include "goomutils/mathutils.h"
#include "stats/sound_stats.h"

#undef NDEBUG
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <format>
#include <limits>
#include <stdexcept>
#include <vector>

namespace GOOM
{

inline auto FloatToInt16(const float f) -> int16_t
{
  if (f >= 1.0F)
  {
    return std::numeric_limits<int16_t>::max();
  }

  if (f < -1.0F)
  {
    return -std::numeric_limits<int16_t>::max();
  }

  return static_cast<int16_t>(f * static_cast<float>(std::numeric_limits<int16_t>::max()));
}

AudioSamples::AudioSamples(const size_t numSampleChannels,
                           const float floatAudioData[NUM_AUDIO_SAMPLES * AUDIO_SAMPLE_LEN])
  : m_numDistinctChannels{numSampleChannels}, m_sampleArrays(NUM_CHANNELS)
{
  if (numSampleChannels == 0 || numSampleChannels > 2)
  {
    throw std::logic_error(
        std20::format("Invalid 'numSampleChannels == {}. Must be '1' or '2'.", numSampleChannels));
  }

  m_sampleArrays[0].resize(AUDIO_SAMPLE_LEN);
  m_sampleArrays[1].resize(AUDIO_SAMPLE_LEN);

  if (NUM_CHANNELS == 1)
  {
    for (size_t i = 0; i < AUDIO_SAMPLE_LEN; i++)
    {
      m_sampleArrays[0][i] = FloatToInt16(floatAudioData[i]);
      m_sampleArrays[1][i] = m_sampleArrays[0][i];
    }
  }
  else
  {
    int fpos = 0;
    for (size_t i = 0; i < AUDIO_SAMPLE_LEN; i++)
    {
      m_sampleArrays[0][i] = FloatToInt16(floatAudioData[fpos]);
      fpos++;
      m_sampleArrays[1][i] = FloatToInt16(floatAudioData[fpos]);
      fpos++;
    }
  }
}

auto AudioSamples::GetSample(const size_t channelIndex) const -> const std::vector<int16_t>&
{
  return m_sampleArrays.at(channelIndex);
}

auto AudioSamples::GetSample(const size_t channelIndex) -> std::vector<int16_t>&
{
  return m_sampleArrays.at(channelIndex);
}

SoundInfo::SoundInfo() noexcept
  : m_stats{std::make_shared<SoundStats>()},
    m_allTimesMaxVolume{std::numeric_limits<int16_t>::min()},
    m_allTimesMinVolume{std::numeric_limits<int16_t>::max()}
{
}

SoundInfo::SoundInfo(const SoundInfo& s) noexcept = default;

SoundInfo::~SoundInfo() noexcept = default;

void SoundInfo::ProcessSample(const AudioSamples& samples)
{
  m_updateNum++;

  UpdateVolume(samples);
  assert(0.0 <= m_volume && m_volume <= 1.0);

  const float prevAcceleration = m_acceleration;
  UpdateAcceleration();
  assert(0.0 <= m_acceleration && m_acceleration <= 1.0);

  UpdateSpeed(prevAcceleration);
  assert(0.0 <= m_speed && m_speed <= 1.0);

  // Detection des nouveaux gooms
  // Detection of new gooms
  UpdateLastBigGoom();
  UpdateLastGoom();
}

void SoundInfo::UpdateVolume(const AudioSamples& samples)
{
  // Find the min/max of volumes
  int16_t maxPosVar = 0;
  int16_t maxVar = std::numeric_limits<int16_t>::min();
  int16_t minVar = std::numeric_limits<int16_t>::max();
  for (size_t n = 0; n < AudioSamples::NUM_CHANNELS; n++)
  {
    const std::vector<int16_t>& soundData = samples.GetSample(n);
    for (int16_t dataVal : soundData)
    {
      if (maxPosVar < dataVal)
      {
        maxPosVar = dataVal;
      }
      if (maxVar < dataVal)
      {
        maxVar = dataVal;
      }
      if (minVar > dataVal)
      {
        minVar = dataVal;
      }
    }
  }

  if (maxPosVar > m_allTimesPositiveMaxVolume)
  {
    m_allTimesPositiveMaxVolume = maxPosVar;
  }
  if (maxVar > m_allTimesMaxVolume)
  {
    m_allTimesMaxVolume = maxVar;
  }
  if (minVar < m_allTimesMinVolume)
  {
    m_allTimesMinVolume = minVar;
  }

  // Volume sonore - TODO: why only positive volumes?
  m_volume = static_cast<float>(maxPosVar) / static_cast<float>(m_allTimesPositiveMaxVolume);

  m_stats->DoUpdateVolume(m_volume);
}

void SoundInfo::UpdateAcceleration()
{
  // Transformations sur la vitesse du son
  // Speed of sound transformations
  m_acceleration = m_volume;

  constexpr float VERY_SLOW_SPEED = 0.1;
  constexpr float SLOW_SPEED = 0.3;

  if (m_speed < VERY_SLOW_SPEED)
  {
    m_acceleration *= (1.0F - static_cast<float>(m_speed));
  }
  else if (m_speed < SLOW_SPEED)
  {
    m_acceleration *= (0.9F - 0.5F * static_cast<float>(m_speed - VERY_SLOW_SPEED));
  }
  else
  {
    m_acceleration *= (0.8F - 0.25F * static_cast<float>(m_speed - SLOW_SPEED));
  }

  // Adoucissement de l'acceleration
  // Smooth acceleration
  if (m_acceleration < 0.0F)
  {
    m_acceleration = 0.0F;
  }
  else
  {
    m_acceleration *= ACCELERATION_MULTIPLIER;
  }

  m_stats->DoUpdateAcceleration(m_acceleration);
}

void SoundInfo::UpdateSpeed(const float prevAcceleration)
{
  // Mise a jour de la vitesse
  // Speed update
  float diffAcceleration = m_acceleration - prevAcceleration;
  if (diffAcceleration < 0.0F)
  {
    diffAcceleration = -diffAcceleration;
  }

  constexpr float LERP_SPEED_MIX = 0.75;
  const float newSpeed = SPEED_MULTIPLIER * (0.5F * m_speed + 0.25F * diffAcceleration);
  m_speed = stdnew::lerp(newSpeed, m_speed, LERP_SPEED_MIX);
  if (m_speed < 0.0F)
  {
    m_speed = 0.0F;
  }
  else if (m_speed > 1.0F)
  {
    m_speed = 1.0F;
  }

  m_stats->DoUpdateSpeed(m_speed);
}

void SoundInfo::UpdateLastGoom()
{
  // Temps du goom
  // Goom time
  m_timeSinceLastGoom++;

  if (m_acceleration > m_goomLimit)
  {
    m_timeSinceLastGoom = 0;
    m_stats->DoGoom();

    // TODO: tester && (info->m_timeSinceLastGoom > 20)) {
    m_totalGoomsInCurrentCycle++;
    m_goomPower = m_acceleration - m_goomLimit;
  }
  if (m_acceleration > m_maxAccelSinceLastReset)
  {
    m_maxAccelSinceLastReset = m_acceleration;
  }

  // Toute les 2 secondes: vérifier si le taux de goom est correct et le modifier sinon.
  // Every 2 seconds: check if the goom rate is correct and modify it otherwise.
  if (m_updateNum % CYCLE_TIME == 0)
  {
    UpdateGoomLimit();
    assert(0.0 <= m_goomLimit && m_goomLimit <= 1.0);

    m_totalGoomsInCurrentCycle = 0;
    m_maxAccelSinceLastReset = 0.0F;
    m_bigGoomLimit = m_goomLimit * BIG_GOOM_FACTOR;
  }

  // m_bigGoomLimit == m_goomLimit*9/8+7 ?
}

void SoundInfo::UpdateLastBigGoom()
{
  m_timeSinceLastBigGoom++;

  if ((m_speed > BIG_GOOM_SPEED_LIMIT) && (m_acceleration > m_bigGoomLimit) &&
      (m_timeSinceLastBigGoom > MAX_BIG_GOOM_DURATION))
  {
    m_timeSinceLastBigGoom = 0;
    m_stats->DoBigGoom();
  }
}

void SoundInfo::UpdateGoomLimit()
{
  constexpr float VERY_SLOW_SPEED = 0.01;
  constexpr float VERY_SLOW_SPEED_FACTOR = 0.91;
  if (m_speed < VERY_SLOW_SPEED)
  {
    m_goomLimit *= VERY_SLOW_SPEED_FACTOR;
  }

  constexpr uint32_t TOTAL_GOOMS_SHORT_CYCLE = 4;
  constexpr float TOTAL_GOOMS_SHORT_INCREMENT = 0.02;
  constexpr uint32_t TOTAL_GOOMS_MEDIUM_CYCLE = 7;
  constexpr float TOTAL_GOOMS_MEDIUM_INCREMENT = 0.03;
  constexpr uint32_t TOTAL_GOOMS_BIG_CYCLE = 16;
  constexpr float TOTAL_GOOMS_BIG_INCREMENT = 0.04;
  constexpr float ACCEL_DECREMENT = 0.02;

  if (m_totalGoomsInCurrentCycle > TOTAL_GOOMS_SHORT_CYCLE)
  {
    m_goomLimit += TOTAL_GOOMS_SHORT_INCREMENT;
  }
  else if (m_totalGoomsInCurrentCycle > TOTAL_GOOMS_MEDIUM_CYCLE)
  {
    m_goomLimit *= 1.0 + TOTAL_GOOMS_MEDIUM_INCREMENT;
    m_goomLimit += TOTAL_GOOMS_MEDIUM_INCREMENT;
  }
  else if (m_totalGoomsInCurrentCycle > TOTAL_GOOMS_BIG_CYCLE)
  {
    m_goomLimit *= 1.0 + TOTAL_GOOMS_BIG_INCREMENT;
    m_goomLimit += TOTAL_GOOMS_BIG_INCREMENT;
  }
  else if (m_totalGoomsInCurrentCycle == 0)
  {
    m_goomLimit = m_maxAccelSinceLastReset - ACCEL_DECREMENT;
  }

  constexpr float TOO_BIG_LIMIT = 0.02;
  constexpr float SMALL_DECREMENT = 0.01;
  if ((m_totalGoomsInCurrentCycle == 1) && (m_goomLimit > TOO_BIG_LIMIT))
  {
    m_goomLimit -= SMALL_DECREMENT;
  }

  constexpr float MIN_LIMIT = 0.0;
  constexpr float MAX_LIMIT = 1.0;
  if (m_goomLimit < MIN_LIMIT)
  {
    m_goomLimit = MIN_LIMIT;
  }
  else if (m_goomLimit > MAX_LIMIT)
  {
    m_goomLimit = MAX_LIMIT;
  }
}

void SoundInfo::Log(const GoomStats::LogStatsValueFunc& l) const
{
  m_stats->LastGoomPower(GetGoomPower());

  m_stats->Log(l);
}

} // namespace GOOM
