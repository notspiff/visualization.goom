#ifndef VISUALIZATION_GOOM_SOUND_INFO_H
#define VISUALIZATION_GOOM_SOUND_INFO_H

#include "goom_config.h"
#include "goom_stats.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace GOOM
{

class SoundStats;

class AudioSamples
{
public:
  static constexpr size_t NUM_CHANNELS = 2;

  // AudioSample object: numSampleChannels = 1 or 2.
  //   If numSampleChannels = 1, then the first  AUDIO_SAMPLE_LEN values of 'floatAudioData'
  //   are used to for two channels.
  //   If numSampleChannels = 2, then the 'floatAudioData' must interleave the two channels,
  //   one after the other. So 'floatAudioData[0]' is channel 0, 'floatAudioData[1]' is
  //   channel 1, 'floatAudioData[2]' is channel 0, 'floatAudioData[3]' is channel 1, etc.
  AudioSamples(size_t numSampleChannels,
               const float floatAudioData[NUM_AUDIO_SAMPLES * AUDIO_SAMPLE_LEN]);

  [[nodiscard]] auto GetNumDistinctChannels() const -> size_t { return m_numDistinctChannels; }
  [[nodiscard]] auto GetSample(size_t channelIndex) const -> const std::vector<int16_t>&;
  auto GetSample(size_t channelIndex) -> std::vector<int16_t>&;

private:
  const size_t m_numDistinctChannels;
  using SampleArray = std::vector<int16_t>;
  std::vector<SampleArray> m_sampleArrays;
};

class SoundInfo
{
public:
  SoundInfo() noexcept;
  ~SoundInfo() noexcept;
  SoundInfo(const SoundInfo& s) noexcept;
  SoundInfo(SoundInfo&&) noexcept = delete;
  auto operator=(const SoundInfo&) -> SoundInfo& = delete;
  auto operator=(SoundInfo&&) -> SoundInfo& = delete;

  void ProcessSample(const AudioSamples& s);

  // Note: a Goom is just a sound event...
  [[nodiscard]] auto GetTimeSinceLastGoom() const -> uint32_t; // >= 0
  [[nodiscard]] auto GetTimeSinceLastBigGoom() const -> uint32_t; // >= 0

  // Number of Gooms since last reset (every 'CYCLE_TIME')
  [[nodiscard]] auto GetTotalGoomsInCurrentCycle() const -> uint32_t;

  // Power of the last Goom [0..1]
  [[nodiscard]] auto GetGoomPower() const -> float;

  // Volume of the sound [0..1]
  [[nodiscard]] auto GetVolume() const -> float;

  // Speed of the sound [0..1]
  [[nodiscard]] auto GetSpeed() const -> float;

  // Acceleration of the sound [0..1]
  [[nodiscard]] auto GetAcceleration() const -> float;

  [[nodiscard]] auto GetAllTimesMaxVolume() const -> int16_t;
  [[nodiscard]] auto GetAllTimesMinVolume() const -> int16_t;

  void Log(const GoomStats::LogStatsValueFunc& l) const;

private:
  std::shared_ptr<SoundStats> m_stats{};

  uint32_t m_updateNum = 0;
  uint32_t m_totalGoomsInCurrentCycle = 0;
  static constexpr uint32_t CYCLE_TIME = 64;
  uint32_t m_timeSinceLastGoom = 0;
  static constexpr uint32_t MAX_BIG_GOOM_DURATION = 100;
  uint32_t m_timeSinceLastBigGoom = 0;
  static constexpr float BIG_GOOM_SPEED_LIMIT = 0.10;
  static constexpr float BIG_GOOM_FACTOR = 1.01;
  float m_goomLimit = 1.0; // auto-updated limit of goom_detection
  float m_bigGoomLimit = 1.0;
  float m_goomPower = 0.0;

  static constexpr float ACCELERATION_MULTIPLIER = 0.95;
  static constexpr float SPEED_MULTIPLIER = 0.99;

  float m_volume = 0.0;
  float m_acceleration = 0.0;
  float m_speed = 0.0;

  int16_t m_allTimesMaxVolume = 0;
  int16_t m_allTimesMinVolume = 0;
  int16_t m_allTimesPositiveMaxVolume = 1;
  float m_maxAccelSinceLastReset = 0.0F;

  void UpdateVolume(const AudioSamples& samples);
  void UpdateAcceleration();
  void UpdateSpeed(float prevAcceleration);
  void UpdateLastGoom();
  void UpdateLastBigGoom();
  void UpdateGoomLimit();
};

inline auto SoundInfo::GetTotalGoomsInCurrentCycle() const -> uint32_t
{
  return m_totalGoomsInCurrentCycle;
}

inline auto SoundInfo::GetTimeSinceLastGoom() const -> uint32_t
{
  return m_timeSinceLastGoom;
}

inline auto SoundInfo::GetTimeSinceLastBigGoom() const -> uint32_t
{
  return m_timeSinceLastBigGoom;
}

inline auto SoundInfo::GetVolume() const -> float
{
  return m_volume;
}

inline auto SoundInfo::GetSpeed() const -> float
{
  return m_speed;
}

inline auto SoundInfo::GetAcceleration() const -> float
{
  return m_acceleration;
}

inline auto SoundInfo::GetAllTimesMaxVolume() const -> int16_t
{
  return m_allTimesMaxVolume;
}

inline auto SoundInfo::GetAllTimesMinVolume() const -> int16_t
{
  return m_allTimesMinVolume;
}

inline auto SoundInfo::GetGoomPower() const -> float
{
  return m_goomPower;
}

} // namespace GOOM
#endif
