#ifndef VISUALIZATION_GOOM_LIB_GOOMUTILS_COLORMAPS_H
#define VISUALIZATION_GOOM_LIB_GOOMUTILS_COLORMAPS_H

#include "goom/goom_graphic.h"
#include "goomutils/colordata/colormap_enums.h"

#include <memory>
#include <utility>

#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

class IColorMap
{
public:
  IColorMap() noexcept = default;
  virtual ~IColorMap() noexcept = default;
  IColorMap(const IColorMap&) noexcept = delete;
  IColorMap(IColorMap&&) noexcept = delete;
  auto operator=(const IColorMap&) -> IColorMap& = delete;
  auto operator=(IColorMap&&) -> IColorMap& = delete;

  [[nodiscard]] virtual auto GetNumStops() const -> size_t = 0;
  [[nodiscard]] virtual auto GetMapName() const -> COLOR_DATA::ColorMapName = 0;

  [[nodiscard]] virtual auto GetColor(float t) const -> Pixel = 0;

  static auto GetColorMix(const Pixel& col1, const Pixel& col2, float t) -> Pixel;

private:
  friend class ColorMaps;
};

class ColorMapWrapper : public IColorMap
{
public:
  ColorMapWrapper() noexcept = delete;
  explicit ColorMapWrapper(std::shared_ptr<const IColorMap> cm) noexcept;
  ~ColorMapWrapper() noexcept override = default;
  ColorMapWrapper(const ColorMapWrapper&) noexcept = delete;
  ColorMapWrapper(ColorMapWrapper&&) noexcept = delete;
  auto operator=(const ColorMapWrapper&) -> ColorMapWrapper& = delete;
  auto operator=(ColorMapWrapper&&) -> ColorMapWrapper& = delete;

  [[nodiscard]] auto GetNumStops() const -> size_t override;
  [[nodiscard]] auto GetMapName() const -> COLOR_DATA::ColorMapName override;
  [[nodiscard]] auto GetColor(float t) const -> Pixel override;

protected:
  [[nodiscard]] auto GetColorMap() const -> std::shared_ptr<const IColorMap> { return m_colorMap; }

private:
  std::shared_ptr<const IColorMap> m_colorMap;
};

enum class ColorMapGroup : int
{
  _NULL = -1,
  PERCEPTUALLY_UNIFORM_SEQUENTIAL = 0,
  SEQUENTIAL,
  SEQUENTIAL2,
  CYCLIC,
  DIVERGING,
  DIVERGING_BLACK,
  QUALITATIVE,
  MISC,
  _SIZE // unused and marks last enum + 1
};

//constexpr size_t to_int(const ColorMapGroup i) { return static_cast<size_t>(i); }
template<class T>
constexpr T& at(std::array<T, static_cast<size_t>(ColorMapGroup::_SIZE)>& arr,
                const ColorMapGroup idx)
{
  return arr[static_cast<size_t>(idx)];
}

class ColorMaps
{
public:
  ColorMaps() noexcept;
  virtual ~ColorMaps() noexcept;
  ColorMaps(const ColorMaps&) noexcept = delete;
  ColorMaps(ColorMaps&&) noexcept = delete;
  auto operator=(const ColorMaps&) -> ColorMaps& = delete;
  auto operator=(ColorMaps&&) -> ColorMaps& = delete;

  [[nodiscard]] auto GetNumColorMapNames() const -> size_t;
  using ColorMapNames = std::vector<COLOR_DATA::ColorMapName>;
  [[nodiscard]] auto GetColorMapNames(ColorMapGroup cmg) const -> const ColorMapNames&;

  [[nodiscard]] auto GetColorMap(COLOR_DATA::ColorMapName mapName) const -> const IColorMap&;

  [[nodiscard]] auto GetColorMapPtr(COLOR_DATA::ColorMapName mapName) const
      -> std::shared_ptr<const IColorMap>;

  [[nodiscard]] auto GetRotatedColorMapPtr(COLOR_DATA::ColorMapName mapName,
                                           float tRotatePoint) const
      -> std::shared_ptr<const IColorMap>;
  [[nodiscard]] auto GetRotatedColorMapPtr(const std::shared_ptr<const IColorMap>& cm,
                                           float tRotatePoint) const
      -> std::shared_ptr<const IColorMap>;

  [[nodiscard]] auto GetTintedColorMapPtr(COLOR_DATA::ColorMapName mapName,
                                          float saturation,
                                          float lightness) const
      -> std::shared_ptr<const IColorMap>;
  [[nodiscard]] auto GetTintedColorMapPtr(const std::shared_ptr<const IColorMap>& cm,
                                          float saturation,
                                          float lightness) const
      -> std::shared_ptr<const IColorMap>;

  [[nodiscard]] auto GetNumGroups() const -> size_t;

private:
  class ColorMapsImpl;
  std::unique_ptr<ColorMapsImpl> m_colorMapsImpl;
};

inline ColorMapWrapper::ColorMapWrapper(std::shared_ptr<const IColorMap> cm) noexcept
  : m_colorMap{std::move(cm)}
{
}

inline auto ColorMapWrapper::GetNumStops() const -> size_t
{
  return m_colorMap->GetNumStops();
}

inline auto ColorMapWrapper::GetMapName() const -> COLOR_DATA::ColorMapName
{
  return m_colorMap->GetMapName();
}

inline auto ColorMapWrapper::GetColor(float t) const -> Pixel
{
  return m_colorMap->GetColor(t);
}


#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
#endif
