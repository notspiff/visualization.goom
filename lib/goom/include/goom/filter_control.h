#ifndef VISUALIZATION_GOOM_FILTER_CONTROL_H
#define VISUALIZATION_GOOM_FILTER_CONTROL_H

#include "filter_data.h"

#include <memory>
#include <string>
#include <vector>

namespace GOOM
{

class PluginInfo;

class FilterControl
{
public:
  explicit FilterControl(const std::shared_ptr<const GOOM::PluginInfo>& goomInfo) noexcept;
  virtual ~FilterControl() noexcept;
  FilterControl(const FilterControl&) noexcept = delete;
  FilterControl(FilterControl&&) noexcept = delete;
  auto operator=(const FilterControl&) -> FilterControl& = delete;
  auto operator=(FilterControl&&) -> FilterControl& = delete;

  [[nodiscard]] auto GetResourcesDirectory() const -> const std::string&;
  void SetResourcesDirectory(const std::string& dirName);

  void Start();

  auto HaveSettingsChangedSinceMark() const -> bool;
  void ClearUnchangedMark();

  auto GetVitesseSetting() const -> const Vitesse&;
  auto GetVitesseSetting() -> Vitesse&;

  void SetNoisifySetting(bool value);
  void SetNoiseFactorSetting(float value);
  void SetBlockyWavySetting(bool value);
  void SetRotateSetting(float value);
  void MultiplyRotateSetting(float factor);
  void ToggleRotateSetting();

  void ChangeMilieu();

  auto GetFilterSettings() const -> const ZoomFilterData&;
  void SetRandomFilterSettings();
  void SetRandomFilterSettings(ZoomFilterMode mode);
  void SetDefaultFilterSettings(ZoomFilterMode mode);

private:
  static const UTILS::Weights<ZoomFilterMode> WEIGHTED_FILTER_EVENTS;
  const std::shared_ptr<const PluginInfo> m_goomInfo;
  ZoomFilterData m_filterData{};
  class FilterEvents;
  std::unique_ptr<FilterEvents> m_filterEvents;
  bool m_hasChanged = false;
  std::string m_resourcesDirectory{};
  static const std::vector<std::string> IMAGE_FILENAMES;
  auto GetImageFilename(const std::string& imageFilename) -> std::string;
  std::vector<std::shared_ptr<ImageDisplacement>> m_imageDisplacements{};

  auto GetNewRandomMode() const -> ZoomFilterMode;

  void SetDefaultSettings();

  void SetAmuletModeSettings();
  void SetCrystalBall0ModeSettings();
  void SetCrystalBall1ModeSettings();
  void SetHypercos0ModeSettings();
  void SetHypercos1ModeSettings();
  void SetHypercos2ModeSettings();
  void SetImageDisplacementModeSettings();
  void SetNormalModeSettings();
  void SetScrunchModeSettings();
  void SetSpeedwayModeSettings();
  void SetWaterModeSettings();
  void SetWaveMode0Settings();
  void SetWaveMode1Settings();
  void SetYOnlyModeSettings();

  struct MinMaxValues
  {
    float minVal;
    float maxVal;
  };

  void SetMiddlePoints();
  void SetPlaneEffects();
  void SetRotate(float probability);
  void SetHypercosEffect(const MinMaxValues& minMaxFreq, const MinMaxValues& minMaxAmp);
  void SetWaveModeSettings(const MinMaxValues& minMaxFreq, const MinMaxValues& minMaxAmp);
  void SetWaveEffect(const MinMaxValues& minMaxFreq, const MinMaxValues& minMaxAmp);
  static auto GetRandomHypercosEffect() -> ZoomFilterData::HypercosEffect;
};

inline auto FilterControl::GetResourcesDirectory() const -> const std::string&
{
  return m_resourcesDirectory;
}

inline void FilterControl::SetResourcesDirectory(const std::string& dirName)
{
  m_resourcesDirectory = dirName;
}

inline auto FilterControl::GetFilterSettings() const -> const ZoomFilterData&
{
  return m_filterData;
}

inline auto FilterControl::HaveSettingsChangedSinceMark() const -> bool
{
  return m_hasChanged;
}

inline void FilterControl::ClearUnchangedMark()
{
  m_hasChanged = false;
}

inline auto FilterControl::GetVitesseSetting() const -> const Vitesse&
{
  return m_filterData.vitesse;
}

inline auto FilterControl::GetVitesseSetting() -> Vitesse&
{
  m_hasChanged = true;
  return m_filterData.vitesse;
}

inline void FilterControl::SetNoisifySetting(const bool value)
{
  m_hasChanged = true;
  m_filterData.noisify = value;
}

inline void FilterControl::SetNoiseFactorSetting(const float value)
{
  m_hasChanged = true;
  m_filterData.noiseFactor = value;
}

inline void FilterControl::SetBlockyWavySetting(const bool value)
{
  m_hasChanged = true;
  m_filterData.blockyWavy = value;
}

inline void FilterControl::SetRotateSetting(const float value)
{
  m_hasChanged = true;
  m_filterData.rotateSpeed = value;
}

inline void FilterControl::MultiplyRotateSetting(const float factor)
{
  m_hasChanged = true;
  m_filterData.rotateSpeed *= factor;
}

inline void FilterControl::ToggleRotateSetting()
{
  m_hasChanged = true;
  m_filterData.rotateSpeed = -m_filterData.rotateSpeed;
}

} // namespace GOOM

#endif //VISUALIZATION_GOOM_FILTER_CONTROL_H
