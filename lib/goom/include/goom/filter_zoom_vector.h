#ifndef VISUALIZATION_GOOM_FILTER_ZOOM_VECTOR_H
#define VISUALIZATION_GOOM_FILTER_ZOOM_VECTOR_H

#include "goom_zoom_vector.h"
#include "v2d.h"

namespace GOOM
{

struct ZoomFilterData;
class FilterStats;

class FilterZoomVector : public IZoomVector
{
public:
  explicit FilterZoomVector(float minNormalizedCoordVal) noexcept;
  ~FilterZoomVector() noexcept override = default;
  FilterZoomVector(const FilterZoomVector&) noexcept = delete;
  FilterZoomVector(FilterZoomVector&&) noexcept = delete;
  auto operator=(const FilterZoomVector&) -> FilterZoomVector& = delete;
  auto operator=(FilterZoomVector&&) -> FilterZoomVector& = delete;

  auto GetFilterStats() const -> FilterStats* override;
  void SetFilterStats(FilterStats& stats) override;

  auto GetFilterSettings() const -> const ZoomFilterData* override;
  void SetFilterSettings(const ZoomFilterData& filterSettings) override;

  auto GetMaxSpeedCoeff() const -> float override;
  void SetMaxSpeedCoeff(float val) override;

  auto GetZoomVector(float xNormalized, float yNormalized) const -> V2dFlt override;

private:
  const ZoomFilterData* m_filterSettings{};
  const float m_minNormalizedCoordVal;
  float m_maxSpeedCoeff;
  mutable FilterStats* m_stats{};

  auto GetCleanedVelocity(const V2dFlt& velocity) const -> V2dFlt;
  auto GetStandardVelocity(float xNormalized, float yNormalized) const -> V2dFlt;
  auto GetSpeedCoeffVelocity(float xNormalized, float yNormalized, float sqDist) const -> V2dFlt;
  auto GetImageDisplacementVelocity(float xNormalized, float yNormalized) const -> V2dFlt;
  auto GetSpeedCoefficient(float xNormalized, float yNormalized, float sqDist) const -> float;
  auto GetWaveEffectSpeedCoeff(float sqDist) const -> float;
  auto GetYOnlySpeedCoeff(float xNormalized, float yNormalized) const -> float;
  auto GetClampedSpeedCoeff(float speedCoeff) const -> float;
  auto GetHypercosVelocity(float xNormalized, float yNormalized) const -> V2dFlt;
  auto GetHPlaneEffectVelocity(float xNormalized, float yNormalized) const -> float;
  auto GetVPlaneEffectVelocity(float xNormalized, float yNormalized) const -> float;
  auto GetNoiseVelocity() const -> V2dFlt;
  auto GetTanEffectVelocity(float sqDist, const V2dFlt& velocity) const -> V2dFlt;
  auto GetRotatedVelocity(const V2dFlt& velocity) const -> V2dFlt;

  void UpdateDoZoomVectorStats() const;
  void UpdateDoZoomVectorNoisifyStats() const;
  void UpdateDoZoomVectorHypercosEffectStats() const;
  void UpdateDoZoomVectorHPlaneEffectStats() const;
  void UpdateDoZoomVectorVPlaneEffectStats() const;
  void UpdateDoZoomVectorSpeedCoeffBelowMinStats() const;
  void UpdateDoZoomVectorSpeedCoeffAboveMaxStats() const;
  void UpdateDoZoomVectorNoiseFactorStats() const;
  void UpdateDoZoomVectorTanEffectStats() const;
  void UpdateDoZoomVectorNegativeRotateStats() const;
  void UpdateDoZoomVectorPositiveRotateStats() const;
};

} // namespace GOOM

#endif //VISUALIZATION_GOOM_FILTER_ZOOM_VECTOR_H
