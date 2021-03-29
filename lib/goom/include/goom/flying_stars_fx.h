#ifndef VISUALIZATION_GOOM_FLYING_STARS_FX_H
#define VISUALIZATION_GOOM_FLYING_STARS_FX_H

#include "goom_stats.h"
#include "goom_visual_fx.h"

#include <memory>
#include <string>

namespace GOOM
{

class PluginInfo;
class PixelBuffer;

namespace UTILS
{
class SmallImageBitmaps;
} // namespace UTILS

class FlyingStarsFx : public IVisualFx
{
public:
  enum class ColorMode
  {
    _NULL = -1,
    MIX_COLORS,
    REVERSE_MIX_COLORS,
    SIMILAR_LOW_COLORS,
    SINE_MIX_COLORS,
  };

  FlyingStarsFx() noexcept = delete;
  explicit FlyingStarsFx(const std::shared_ptr<const PluginInfo>&) noexcept;
  ~FlyingStarsFx() noexcept override;
  FlyingStarsFx(const FlyingStarsFx&) noexcept = delete;
  FlyingStarsFx(FlyingStarsFx&&) noexcept = delete;
  auto operator=(const FlyingStarsFx&) -> FlyingStarsFx& = delete;
  auto operator=(FlyingStarsFx&&) -> FlyingStarsFx& = delete;

  [[nodiscard]] auto GetResourcesDirectory() const -> const std::string& override;
  void SetResourcesDirectory(const std::string& dirName) override;

  [[nodiscard]] auto GetFxName() const -> std::string override;
  void SetBuffSettings(const FXBuffSettings& settings) override;

  void SetSmallImageBitmaps(const UTILS::SmallImageBitmaps& smallBitmaps);

  void Start() override;

  void Apply(PixelBuffer& currentBuff, PixelBuffer& nextBuff);

  void Log(const GoomStats::LogStatsValueFunc& l) const override;
  void Finish() override;

private:
  bool m_enabled = true;
  class FlyingStarsImpl;
  const std::unique_ptr<FlyingStarsImpl> m_fxImpl;
};

} // namespace GOOM
#endif
