#ifndef VISUALIZATION_GOOM_FILTERS_H
#define VISUALIZATION_GOOM_FILTERS_H

#include "goom_stats.h"
#include "goom_visual_fx.h"

#include <cstdint>
#include <memory>
#include <string>

namespace GOOM
{

namespace UTILS
{
class Parallel;
} // namespace UTILS

class PluginInfo;
class PixelBuffer;
class IZoomVector;
struct ZoomFilterData;

class ZoomFilterFx : public IVisualFx
{
public:
  ZoomFilterFx() noexcept = delete;
  ZoomFilterFx(UTILS::Parallel&, const std::shared_ptr<const PluginInfo>&) noexcept;
  ~ZoomFilterFx() noexcept override;
  ZoomFilterFx(const ZoomFilterFx&) noexcept = delete;
  ZoomFilterFx(ZoomFilterFx&&) noexcept = delete;
  auto operator=(const ZoomFilterFx&) -> ZoomFilterFx& = delete;
  auto operator=(ZoomFilterFx&&) -> ZoomFilterFx& = delete;

  [[nodiscard]] auto GetResourcesDirectory() const -> const std::string& override;
  void SetResourcesDirectory(const std::string& dirName) override;

  [[nodiscard]] auto GetFxName() const -> std::string override;
  void SetBuffSettings(const FXBuffSettings& settings) override;

  auto GetZoomVector() const -> IZoomVector&;
  void SetZoomVector(IZoomVector& zoomVector);

  void Start() override;

  auto GetFilterSettings() const -> const ZoomFilterData&;
  auto GetFilterSettingsArePending() const -> bool;

  auto GetTranLerpFactor() const -> int32_t;

  void ChangeFilterSettings(const ZoomFilterData& filterSettings);

  void ZoomFilterFastRgb(const PixelBuffer& pix1,
                         PixelBuffer& pix2,
                         int switchIncr,
                         float switchMult,
                         uint32_t& numClipped);

  void Log(const GoomStats::LogStatsValueFunc& l) const override;
  void Finish() override;

private:
  bool m_enabled = true;
  class ZoomFilterImpl;
  const std::unique_ptr<ZoomFilterImpl> m_fxImpl;
};

} // namespace GOOM
#endif
