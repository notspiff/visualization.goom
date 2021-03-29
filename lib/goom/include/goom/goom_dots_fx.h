#ifndef VISUALIZATION_GOOM_GOOM_DOTS_FX_H
#define VISUALIZATION_GOOM_GOOM_DOTS_FX_H

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

class GoomDotsFx : public IVisualFx
{
public:
  GoomDotsFx() noexcept = delete;
  explicit GoomDotsFx(const std::shared_ptr<const PluginInfo>&) noexcept;
  ~GoomDotsFx() noexcept override;
  GoomDotsFx(const GoomDotsFx&) noexcept = delete;
  GoomDotsFx(GoomDotsFx&&) noexcept = delete;
  auto operator=(const GoomDotsFx&) -> GoomDotsFx& = delete;
  auto operator=(GoomDotsFx&&) -> GoomDotsFx& = delete;

  [[nodiscard]] auto GetResourcesDirectory() const -> const std::string& override;
  void SetResourcesDirectory(const std::string& dirName) override;

  [[nodiscard]] auto GetFxName() const -> std::string override;
  void SetBuffSettings(const FXBuffSettings& settings) override;

  void SetSmallImageBitmaps(const UTILS::SmallImageBitmaps& smallBitmaps);

  void Start() override;

  void Apply(PixelBuffer& currentBuff);
  void Apply(PixelBuffer& currentBuff, PixelBuffer& nextBuff);

  void Log(const GoomStats::LogStatsValueFunc& l) const override;
  void Finish() override;

private:
  bool m_enabled = true;
  class GoomDotsFxImpl;
  const std::unique_ptr<GoomDotsFxImpl> m_fxImpl;
};

} // namespace GOOM

#endif /* VISUALIZATION_GOOM_GOOM_DOTS_FX_H */
