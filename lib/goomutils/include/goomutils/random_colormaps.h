#ifndef VISUALIZATION_GOOM_LIB_GOOMUTILS_RANDOM_COLORMAP_H
#define VISUALIZATION_GOOM_LIB_GOOMUTILS_RANDOM_COLORMAP_H

#include "goom/goom_graphic.h"
#include "goomutils/colordata/colormap_enums.h"
#include "goomutils/colormaps.h"
#include "goomutils/goomrand.h"

#include <array>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

class RandomColorMaps : public ColorMaps
{
public:
  RandomColorMaps() noexcept;
  ~RandomColorMaps() noexcept override;
  RandomColorMaps(const RandomColorMaps&) noexcept = delete;
  RandomColorMaps(RandomColorMaps&&) noexcept = delete;
  auto operator=(const RandomColorMaps&) -> RandomColorMaps& = delete;
  auto operator=(RandomColorMaps&&) -> RandomColorMaps& = delete;

  [[nodiscard]] auto GetRandomColorMapName() const -> COLOR_DATA::ColorMapName;
  [[nodiscard]] auto GetRandomColorMapName(ColorMapGroup cmg) const -> COLOR_DATA::ColorMapName;

  [[nodiscard]] auto GetRandomColorMap() const -> const IColorMap&;
  [[nodiscard]] auto GetRandomColorMap(ColorMapGroup cmg) const -> const IColorMap&;

  enum class ColorMapTypes
  {
    ROTATED_T,
    SHADES,
  };
  static const std::set<ColorMapTypes> EMPTY;
  static const std::set<ColorMapTypes> ALL;
  [[nodiscard]] auto GetRandomColorMapPtr(const std::set<ColorMapTypes>& types = EMPTY) const
      -> std::shared_ptr<const IColorMap>;
  [[nodiscard]] auto GetRandomColorMapPtr(COLOR_DATA::ColorMapName cmName,
                                          const std::set<ColorMapTypes>& types = EMPTY) const
      -> std::shared_ptr<const IColorMap>;
  [[nodiscard]] auto GetRandomColorMapPtr(ColorMapGroup cmg,
                                          const std::set<ColorMapTypes>& types = EMPTY) const
      -> std::shared_ptr<const IColorMap>;
  [[nodiscard]] auto GetRandomColorMapPtr(const std::shared_ptr<const IColorMap>& cm,
                                          const std::set<ColorMapTypes>& types) const
      -> std::shared_ptr<const IColorMap>;

  [[nodiscard]] auto GetMinRotationPoint() const -> float;
  [[nodiscard]] auto GetMaxRotationPoint() const -> float;
  void SetRotationPointLimits(float minRotationPoint, float maxRotationPoint);

  [[nodiscard]] auto GetRandomRotatedColorMapPtr() const -> std::shared_ptr<const IColorMap>;
  [[nodiscard]] auto GetRandomRotatedColorMapPtr(COLOR_DATA::ColorMapName cmName) const
      -> std::shared_ptr<const IColorMap>;
  [[nodiscard]] auto GetRandomRotatedColorMapPtr(ColorMapGroup cmg) const
      -> std::shared_ptr<const IColorMap>;
  [[nodiscard]] auto GetRandomRotatedColorMapPtr(const std::shared_ptr<const IColorMap>& cm) const
      -> std::shared_ptr<const IColorMap>;

  [[nodiscard]] auto GetMinSaturation() const -> float;
  [[nodiscard]] auto GetMaxSaturation() const -> float;
  void SetSaturationLimts(float minSaturation, float maxSaturation);
  [[nodiscard]] auto GetMaxLightness() const -> float;
  [[nodiscard]] auto GetMinLightness() const -> float;
  void SetLightnessLimits(float minLightness, float maxLightness);

  [[nodiscard]] auto GetRandomTintedColorMapPtr() const -> std::shared_ptr<const IColorMap>;
  [[nodiscard]] auto GetRandomTintedColorMapPtr(COLOR_DATA::ColorMapName cmName) const
      -> std::shared_ptr<const IColorMap>;
  [[nodiscard]] auto GetRandomTintedColorMapPtr(ColorMapGroup cmg) const
      -> std::shared_ptr<const IColorMap>;
  [[nodiscard]] auto GetRandomTintedColorMapPtr(const std::shared_ptr<const IColorMap>& cm) const
      -> std::shared_ptr<const IColorMap>;

  [[nodiscard]] virtual auto GetRandomGroup() const -> ColorMapGroup;

  [[nodiscard]] static auto GetRandomColor(const IColorMap& colorMap, float t0, float t1) -> Pixel;

private:
#if __cplusplus <= 201402L
  static const float MIN_ROTATION_POINT;
  static const float MAX_ROTATION_POINT;
#else
  static constexpr float MIN_ROTATION_POINT = 0.1F;
  static constexpr float MAX_ROTATION_POINT = 0.9F;
#endif
  float m_minRotationPoint = MIN_ROTATION_POINT;
  float m_maxRotationPoint = MAX_ROTATION_POINT;

#if __cplusplus <= 201402L
  static const float MIN_SATURATION;
  static const float MAX_SATURATION;
  static const float MIN_LIGHTNESS;
  static const float MAX_LIGHTNESS;
#else
  static constexpr float MIN_SATURATION = 0.1F;
  static constexpr float MAX_SATURATION = 1.0F;
  static constexpr float MIN_LIGHTNESS = 0.1F;
  static constexpr float MAX_LIGHTNESS = 1.0F;
#endif
  float m_minSaturation = MIN_SATURATION;
  float m_maxSaturation = MAX_SATURATION;
  float m_minLightness = MIN_LIGHTNESS;
  float m_maxLightness = MAX_LIGHTNESS;
};

class WeightedColorMaps : public RandomColorMaps
{
public:
  WeightedColorMaps();
  explicit WeightedColorMaps(const Weights<ColorMapGroup>&);
  ~WeightedColorMaps() noexcept override = default;
  WeightedColorMaps(const WeightedColorMaps&) noexcept = delete;
  WeightedColorMaps(WeightedColorMaps&&) noexcept = delete;
  auto operator=(const WeightedColorMaps&) -> WeightedColorMaps& = delete;
  auto operator=(WeightedColorMaps&&) -> WeightedColorMaps& = delete;

  [[nodiscard]] auto GetWeights() const -> const Weights<ColorMapGroup>&;
  void SetWeights(const Weights<ColorMapGroup>&);

  [[nodiscard]] auto AreWeightsActive() const -> bool;
  void SetWeightsActive(bool value);

  [[nodiscard]] auto GetRandomGroup() const -> ColorMapGroup override;

private:
  Weights<ColorMapGroup> m_weights;
  bool m_weightsActive;
};

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
#endif
