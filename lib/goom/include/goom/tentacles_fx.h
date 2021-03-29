#ifndef VISUALIZATION_GOOM_TENTACLES_FX_H
#define VISUALIZATION_GOOM_TENTACLES_FX_H

#include "goom_stats.h"
#include "goom_visual_fx.h"

#include <memory>
#include <string>

namespace GOOM
{

class PluginInfo;
class PixelBuffer;

class TentaclesFx : public IVisualFx
{
public:
  TentaclesFx() noexcept = delete;
  explicit TentaclesFx(const std::shared_ptr<const PluginInfo>&) noexcept;
  ~TentaclesFx() noexcept override;
  TentaclesFx(const TentaclesFx&) noexcept = delete;
  TentaclesFx(TentaclesFx&&) noexcept = delete;
  auto operator=(const TentaclesFx&) -> TentaclesFx& = delete;
  auto operator=(TentaclesFx&&) -> TentaclesFx& = delete;

  [[nodiscard]] auto GetResourcesDirectory() const -> const std::string& override;
  void SetResourcesDirectory(const std::string& dirName) override;

  [[nodiscard]] auto GetFxName() const -> std::string override;
  void SetBuffSettings(const FXBuffSettings& settings) override;

  void FreshStart();

  void Start() override;

  void ApplyNoDraw();
  void Apply(PixelBuffer& currentBuff, PixelBuffer& nextBuff);

  void Log(const GoomStats::LogStatsValueFunc& l) const override;
  void Finish() override;

private:
  bool m_enabled = true;
  class TentaclesImpl;
  const std::unique_ptr<TentaclesImpl> m_fxImpl;
};

} // namespace GOOM
#endif
