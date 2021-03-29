#include "random_colormaps.h"

#include "colordata/colormap_enums.h"
#include "colormaps.h"
#include "goom/goom_graphic.h"
#include "goomrand.h"

#include <format>
#include <memory>
#include <set>
#include <stdexcept>

#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

using COLOR_DATA::ColorMapName;

#if __cplusplus <= 201402L
const float RandomColorMaps::MIN_ROTATION_POINT = 0.1F;
const float RandomColorMaps::MAX_ROTATION_POINT = 0.9F;
const float RandomColorMaps::MIN_SATURATION = 0.1F;
const float RandomColorMaps::MAX_SATURATION = 1.0F;
const float RandomColorMaps::MIN_LIGHTNESS = 0.1F;
const float RandomColorMaps::MAX_LIGHTNESS = 1.0F;
#endif

const std::set<RandomColorMaps::ColorMapTypes> RandomColorMaps::EMPTY{};
const std::set<RandomColorMaps::ColorMapTypes> RandomColorMaps::ALL{
    ColorMapTypes::ROTATED_T,
    ColorMapTypes::SHADES,
};

RandomColorMaps::RandomColorMaps() noexcept = default;

RandomColorMaps::~RandomColorMaps() noexcept = default;

auto RandomColorMaps::GetRandomColorMapName() const -> COLOR_DATA::ColorMapName
{
  return static_cast<ColorMapName>(GetRandInRange(0U, GetNumColorMapNames()));
}

auto RandomColorMaps::GetRandomColorMapName(const ColorMapGroup cmg) const
    -> COLOR_DATA::ColorMapName
{
  const ColorMapNames& colorMapNames = GetColorMapNames(cmg);
  return colorMapNames[GetRandInRange(0U, colorMapNames.size())];
}

auto RandomColorMaps::GetRandomColorMap() const -> const IColorMap&
{
  return GetColorMap(GetRandomColorMapName());
}

auto RandomColorMaps::GetRandomColorMap(const ColorMapGroup cmg) const -> const IColorMap&
{
  return GetColorMap(GetRandomColorMapName(cmg));
}

auto RandomColorMaps::GetRandomColorMapPtr(const std::set<ColorMapTypes>& types) const
    -> std::shared_ptr<const IColorMap>
{
  return GetRandomColorMapPtr(GetColorMapPtr(GetRandomColorMapName()), types);
}

auto RandomColorMaps::GetRandomColorMapPtr(const COLOR_DATA::ColorMapName cmName,
                                           const std::set<ColorMapTypes>& types) const
    -> std::shared_ptr<const IColorMap>
{
  return GetRandomColorMapPtr(GetColorMapPtr(cmName), types);
}

auto RandomColorMaps::GetRandomColorMapPtr(const ColorMapGroup cmg,
                                           const std::set<ColorMapTypes>& types) const
    -> std::shared_ptr<const IColorMap>
{
  return GetRandomColorMapPtr(GetColorMapPtr(GetRandomColorMapName(cmg)), types);
}

auto RandomColorMaps::GetRandomColorMapPtr(const std::shared_ptr<const IColorMap>& cm,
                                           const std::set<ColorMapTypes>& types) const
    -> std::shared_ptr<const IColorMap>
{
  if (types.empty())
  {
    return cm;
  }

  std::shared_ptr<const IColorMap> colorMap = cm;

#if __cplusplus <= 201402L
  if (types.find(ColorMapTypes::ROTATED_T) != types.end())
#else
  if (types.contains(ColorMapTypes::ROTATED_T))
#endif
  {
    colorMap = GetRandomRotatedColorMapPtr(colorMap);
  }
#if __cplusplus <= 201402L
  if (types.find(ColorMapTypes::SHADES) != types.end())
#else
  if (types.contains(ColorMapTypes::SHADES))
#endif
  {
    colorMap = GetRandomTintedColorMapPtr(colorMap);
  }

  return colorMap;
}

auto RandomColorMaps::GetRandomRotatedColorMapPtr() const -> std::shared_ptr<const IColorMap>
{
  return GetRotatedColorMapPtr(GetRandomColorMapName(),
                               GetRandInRange(m_minRotationPoint, m_maxRotationPoint));
}

auto RandomColorMaps::GetRandomRotatedColorMapPtr(const COLOR_DATA::ColorMapName cmName) const
    -> std::shared_ptr<const IColorMap>
{
  return GetRotatedColorMapPtr(cmName, GetRandInRange(m_minRotationPoint, m_maxRotationPoint));
}

auto RandomColorMaps::GetRandomRotatedColorMapPtr(const ColorMapGroup cmg) const
    -> std::shared_ptr<const IColorMap>
{
  return GetRotatedColorMapPtr(GetRandomColorMapName(cmg),
                               GetRandInRange(m_minRotationPoint, m_maxRotationPoint));
}

auto RandomColorMaps::GetRandomRotatedColorMapPtr(const std::shared_ptr<const IColorMap>& cm) const
    -> std::shared_ptr<const IColorMap>
{
  return GetRotatedColorMapPtr(cm, GetRandInRange(m_minRotationPoint, m_maxRotationPoint));
}

auto RandomColorMaps::GetRandomTintedColorMapPtr() const -> std::shared_ptr<const IColorMap>
{
  return GetTintedColorMapPtr(GetRandomColorMapName(),
                              GetRandInRange(m_minSaturation, m_maxSaturation),
                              GetRandInRange(m_minLightness, m_maxLightness));
}

auto RandomColorMaps::GetRandomTintedColorMapPtr(const ColorMapName cmName) const
    -> std::shared_ptr<const IColorMap>
{
  return GetTintedColorMapPtr(cmName, GetRandInRange(m_minSaturation, m_maxSaturation),
                              GetRandInRange(m_minLightness, m_maxLightness));
}

auto RandomColorMaps::GetRandomTintedColorMapPtr(const ColorMapGroup cmg) const
    -> std::shared_ptr<const IColorMap>
{
  return GetTintedColorMapPtr(GetRandomColorMapName(cmg),
                              GetRandInRange(m_minSaturation, m_maxSaturation),
                              GetRandInRange(m_minLightness, m_maxLightness));
}

auto RandomColorMaps::GetRandomTintedColorMapPtr(const std::shared_ptr<const IColorMap>& cm) const
    -> std::shared_ptr<const IColorMap>
{
  return GetTintedColorMapPtr(cm, GetRandInRange(m_minSaturation, m_maxSaturation),
                              GetRandInRange(m_minLightness, m_maxLightness));
}

auto RandomColorMaps::GetRandomGroup() const -> ColorMapGroup
{
  return static_cast<ColorMapGroup>(GetRandInRange(0U, static_cast<size_t>(ColorMapGroup::_SIZE)));
}

auto RandomColorMaps::GetRandomColor(const IColorMap& colorMap, float t0, float t1) -> Pixel
{
  return colorMap.GetColor(GetRandInRange(t0, t1));
}

auto RandomColorMaps::GetMinRotationPoint() const -> float
{
  return m_minRotationPoint;
}

auto RandomColorMaps::GetMaxRotationPoint() const -> float
{
  return m_maxRotationPoint;
}

void RandomColorMaps::SetRotationPointLimits(float minRotationPoint, float maxRotationPoint)
{
  if (minRotationPoint < MIN_ROTATION_POINT)
  {
    throw std::logic_error(
        std20::format("minRotationPoint {} < {}", minRotationPoint, MIN_ROTATION_POINT));
  }
  if (maxRotationPoint > MAX_ROTATION_POINT)
  {
    throw std::logic_error(
        std20::format("maxRotationPoint {} < {}", maxRotationPoint, MAX_ROTATION_POINT));
  }
  if (minRotationPoint > maxRotationPoint)
  {
    throw std::logic_error(std20::format("minRotationPoint {} > maxRotationPoint {}",
                                         minRotationPoint, maxRotationPoint));
  }

  m_minRotationPoint = minRotationPoint;
  m_maxRotationPoint = maxRotationPoint;
}

auto RandomColorMaps::GetMinSaturation() const -> float
{
  return m_minSaturation;
}

auto RandomColorMaps::GetMaxSaturation() const -> float
{
  return m_maxSaturation;
}

void RandomColorMaps::SetSaturationLimts(float minSaturation, float maxSaturation)
{
  if (minSaturation < MIN_SATURATION)
  {
    throw std::logic_error(std20::format("minSaturation {} < {}", minSaturation, MIN_SATURATION));
  }
  if (maxSaturation > MAX_SATURATION)
  {
    throw std::logic_error(std20::format("maxSaturation {} < {}", maxSaturation, MAX_SATURATION));
  }
  if (minSaturation > maxSaturation)
  {
    throw std::logic_error(
        std20::format("minSaturation {} > maxSaturation {}", minSaturation, maxSaturation));
  }

  m_minSaturation = minSaturation;
  m_maxSaturation = maxSaturation;
}

auto RandomColorMaps::GetMinLightness() const -> float
{
  return m_minLightness;
}

auto RandomColorMaps::GetMaxLightness() const -> float
{
  return m_maxLightness;
}

void RandomColorMaps::SetLightnessLimits(float minLightness, float maxLightness)
{
  if (minLightness < MIN_LIGHTNESS)
  {
    throw std::logic_error(std20::format("minLightness {} < {}", minLightness, MIN_LIGHTNESS));
  }
  if (maxLightness > MAX_LIGHTNESS)
  {
    throw std::logic_error(std20::format("maxLightness {} < {}", maxLightness, MAX_LIGHTNESS));
  }
  if (minLightness > maxLightness)
  {
    throw std::logic_error(
        std20::format("minLightness {} > maxLightness {}", minLightness, maxLightness));
  }

  m_minLightness = minLightness;
  m_maxLightness = maxLightness;
}

WeightedColorMaps::WeightedColorMaps() : RandomColorMaps{}, m_weights{}, m_weightsActive{true}
{
}

WeightedColorMaps::WeightedColorMaps(const Weights<ColorMapGroup>& w)
  : RandomColorMaps{}, m_weights{w}, m_weightsActive{true}
{
}

auto WeightedColorMaps::GetWeights() const -> const Weights<ColorMapGroup>&
{
  return m_weights;
}

void WeightedColorMaps::SetWeights(const Weights<ColorMapGroup>& w)
{
  m_weights = w;
}

auto WeightedColorMaps::AreWeightsActive() const -> bool
{
  return m_weightsActive;
}

void WeightedColorMaps::SetWeightsActive(const bool value)
{
  m_weightsActive = value;
}

auto WeightedColorMaps::GetRandomGroup() const -> ColorMapGroup
{
  if (!m_weightsActive)
  {
    return RandomColorMaps::GetRandomGroup();
  }

  return m_weights.GetRandomWeighted();
}

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
