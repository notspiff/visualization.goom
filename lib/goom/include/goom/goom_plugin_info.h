#ifndef VISUALIZATION_GOOM_GOOM_PLUGIN_INFO_H
#define VISUALIZATION_GOOM_GOOM_PLUGIN_INFO_H

#include "goom_config.h"
#include "sound_info.h"

#include <cstdint>
#include <memory>

namespace GOOM
{

class PluginInfo
{
public:
  struct Screen
  {
    uint32_t width;
    uint32_t height;
    uint32_t size; // == screen.height * screen.width.
  };

  PluginInfo() noexcept = delete;
  virtual ~PluginInfo() noexcept = default;
  PluginInfo(uint32_t width, uint32_t height) noexcept;
  PluginInfo(const PluginInfo& p) noexcept;
  PluginInfo(PluginInfo&&) noexcept = delete;
  auto operator=(const PluginInfo&) -> PluginInfo& = delete;
  auto operator=(PluginInfo&&) -> PluginInfo& = delete;

  [[nodiscard]] auto GetScreenInfo() const -> const Screen&;
  [[nodiscard]] auto GetSoundInfo() const -> const SoundInfo&;

protected:
  virtual void ProcessSoundSample(const AudioSamples& soundData);

private:
  const Screen m_screen;
  const std::unique_ptr<SoundInfo> m_soundInfo;
};

class WritablePluginInfo : public PluginInfo
{
public:
  WritablePluginInfo(uint32_t width, uint32_t height) noexcept;
  void ProcessSoundSample(const AudioSamples& soundData) override;
};

inline PluginInfo::PluginInfo(const uint32_t width, const uint32_t height) noexcept
  : m_screen{width, height, width * height}, m_soundInfo{std::make_unique<SoundInfo>()}
{
}

inline PluginInfo::PluginInfo(const PluginInfo& p) noexcept
  : m_screen{p.m_screen}, m_soundInfo{new SoundInfo{*p.m_soundInfo}}
{
}

inline auto PluginInfo::GetScreenInfo() const -> const PluginInfo::Screen&
{
  return m_screen;
}

inline auto PluginInfo::GetSoundInfo() const -> const SoundInfo&
{
  return *m_soundInfo;
}

inline void PluginInfo::ProcessSoundSample(const AudioSamples& soundData)
{
  m_soundInfo->ProcessSample(soundData);
}

inline WritablePluginInfo::WritablePluginInfo(const uint32_t width, const uint32_t height) noexcept
  : PluginInfo{width, height}
{
}

inline void WritablePluginInfo::ProcessSoundSample(const AudioSamples& soundData)
{
  PluginInfo::ProcessSoundSample(soundData);
}

} // namespace GOOM
#endif
