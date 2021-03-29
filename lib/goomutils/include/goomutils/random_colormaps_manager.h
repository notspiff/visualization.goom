#ifndef VISUALIZATION_GOOM_RANDOM_COLORMAPS_MANAGER_H
#define VISUALIZATION_GOOM_RANDOM_COLORMAPS_MANAGER_H

#include "colormaps.h"
#include "random_colormaps.h"

#include <memory>
#include <set>

#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

class RandomColorMapsManager
{
public:
  RandomColorMapsManager() noexcept;
  virtual ~RandomColorMapsManager() noexcept;
  RandomColorMapsManager(const RandomColorMapsManager&) noexcept = delete;
  RandomColorMapsManager(RandomColorMapsManager&&) noexcept = delete;
  auto operator=(const RandomColorMapsManager&) -> RandomColorMapsManager& = delete;
  auto operator=(RandomColorMapsManager&&) -> RandomColorMapsManager& = delete;

  void IncCounter();
  void ResetCounter();

  struct ColorMapInfo
  {
    std::shared_ptr<const RandomColorMaps> colorMaps{};
    COLOR_DATA::ColorMapName colorMapName{COLOR_DATA::ColorMapName::_NULL};
    std::set<RandomColorMaps::ColorMapTypes> types{};
    // uint32_t countForNextChange{};
  };

  [[nodiscard]] auto GetColorMapInfo(uint32_t id) const -> const ColorMapInfo&;
  auto AddColorMapInfo(const ColorMapInfo& info) -> uint32_t;

  void UpdateAllColorMapInfo(const ColorMapInfo& info);
  void UpdateColorMapInfo(uint32_t id, const ColorMapInfo& info);

  void UpdateAllColorMapNames(COLOR_DATA::ColorMapName colorMapName);
  void UpdateColorMapName(uint32_t id, COLOR_DATA::ColorMapName colorMapName);

  void UpdateAllColorMapTypes(const std::set<RandomColorMaps::ColorMapTypes>& types);
  void UpdateColorMapTypes(uint32_t id, const std::set<RandomColorMaps::ColorMapTypes>& types);

  void ChangeAllColorMapsNow();
  void ChangeColorMapNow(uint32_t id);

  [[nodiscard]] auto GetColorMap(uint32_t id) const -> const IColorMap&;
  [[nodiscard]] auto GetColorMapPtr(uint32_t id) const -> std::shared_ptr<const IColorMap>;

private:
  std::vector<ColorMapInfo> m_infoList{};
  std::vector<std::shared_ptr<const IColorMap>> m_colorMaps{};
  uint32_t m_counter{};

  void DoUpdateColorMap(uint32_t id, const ColorMapInfo& info);
  void DoUpdateColorMapName(uint32_t id, COLOR_DATA::ColorMapName colorMapName);
  void DoUpdateColorMapTypes(uint32_t id, const std::set<RandomColorMaps::ColorMapTypes>& types);
  void DoChangeColorMap(uint32_t id);
};

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
#endif
