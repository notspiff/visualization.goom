#ifndef VISUALIZATION_GOOM_GOOM_ZOOM_VECTOR_H
#define VISUALIZATION_GOOM_GOOM_ZOOM_VECTOR_H

#include "v2d.h"

namespace GOOM
{

struct ZoomFilterData;
class FilterStats;

class IZoomVector
{
public:
  IZoomVector() noexcept = default;
  virtual ~IZoomVector() noexcept = default;
  IZoomVector(const IZoomVector&) noexcept = delete;
  IZoomVector(IZoomVector&&) noexcept = delete;
  auto operator=(const IZoomVector&) -> IZoomVector& = delete;
  auto operator=(IZoomVector&&) -> IZoomVector& = delete;

  virtual auto GetFilterStats() const -> FilterStats* = 0;
  virtual void SetFilterStats(FilterStats& stats) = 0;

  virtual auto GetFilterSettings() const -> const ZoomFilterData* = 0;
  virtual void SetFilterSettings(const ZoomFilterData& filterSettings) = 0;

  virtual auto GetMaxSpeedCoeff() const -> float = 0;
  virtual void SetMaxSpeedCoeff(float val) = 0;

  virtual auto GetZoomVector(float xNormalized, float yNormalized) const -> V2dFlt = 0;
};

} // namespace GOOM

#endif //VISUALIZATION_GOOM_GOOM_ZOOM_VECTOR_H
