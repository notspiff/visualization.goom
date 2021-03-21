#include "filter_zoom_vector.h"

#include "filter_data.h"
#include "goomutils/mathutils.h"
#include "image_displacement.h"
#include "stats/filter_stats.h"

namespace GOOM
{

using UTILS::GetRandInRange;
using UTILS::m_half_pi;
using UTILS::SMALL_FLOAT;
using UTILS::SqDistance;

FilterZoomVector::FilterZoomVector(const float minNormalizedCoordVal) noexcept
  : m_minNormalizedCoordVal{minNormalizedCoordVal},
    m_maxSpeedCoeff{ZoomFilterData::DEFAULT_MAX_SPEED_COEFF}
{
}

auto FilterZoomVector::GetFilterStats() const -> FilterStats*
{
  return m_stats;
}

void FilterZoomVector::SetFilterStats(FilterStats& stats)
{
  m_stats = &stats;
}

auto FilterZoomVector::GetFilterSettings() const -> const ZoomFilterData*
{
  return m_filterSettings;
}

void FilterZoomVector::SetFilterSettings(const ZoomFilterData& filterSettings)
{
  m_filterSettings = &filterSettings;
}

auto FilterZoomVector::GetMaxSpeedCoeff() const -> float
{
  return m_maxSpeedCoeff;
}

void FilterZoomVector::SetMaxSpeedCoeff(const float val)
{
  m_maxSpeedCoeff = val;
}

auto FilterZoomVector::GetZoomVector(const float xNormalized, const float yNormalized) const
    -> V2dFlt
{
  UpdateDoZoomVectorStats();

  V2dFlt velocity = GetStandardVelocity(xNormalized, yNormalized);

  // The Effects add-ons...
  if (m_filterSettings->noisify)
  {
    UpdateDoZoomVectorNoisifyStats();
    velocity += GetNoiseVelocity();
  }
  if (m_filterSettings->hypercosEffect != ZoomFilterData::HypercosEffect::NONE)
  {
    UpdateDoZoomVectorHypercosEffectStats();
    velocity += GetHypercosVelocity(xNormalized, yNormalized);
  }
  if (m_filterSettings->hPlaneEffect != 0)
  {
    UpdateDoZoomVectorHPlaneEffectStats();
    velocity.x += GetHPlaneEffectVelocity(xNormalized, yNormalized);
  }
  if (m_filterSettings->vPlaneEffect != 0)
  {
    UpdateDoZoomVectorVPlaneEffectStats();
    velocity.y += GetVPlaneEffectVelocity(xNormalized, yNormalized);
  }
  /* TODO : Water Mode */
  //    if (data->waveEffect)

  /**
  if (m_filterSettings->mode == ZoomFilterMode::NORMAL_MODE)
    if (ProbabilityOfMInN(1, 2))
      velocity = {-2.0F * xNormalized + velocity.x, -2.0F * yNormalized + velocity.y};
  **/

  return GetCleanedVelocity(velocity);
}

inline auto FilterZoomVector::GetCleanedVelocity(const V2dFlt& velocity) const -> V2dFlt
{
  V2dFlt newVelocity = velocity;
  if (std::fabs(velocity.x) < m_minNormalizedCoordVal)
  {
    newVelocity.x = (newVelocity.x < 0.0F) ? -m_minNormalizedCoordVal : m_minNormalizedCoordVal;
  }
  if (std::fabs(newVelocity.y) < m_minNormalizedCoordVal)
  {
    newVelocity.y = (newVelocity.y < 0.0F) ? -m_minNormalizedCoordVal : m_minNormalizedCoordVal;
  }
  return newVelocity;
}

inline auto FilterZoomVector::GetStandardVelocity(const float xNormalized,
                                                  const float yNormalized) const -> V2dFlt
{
  const float sqDist = SqDistance(xNormalized, yNormalized);

  V2dFlt velocity = m_filterSettings->mode == ZoomFilterMode::IMAGE_DISPLACEMENT_MODE
                        ? GetImageDisplacementVelocity(xNormalized, yNormalized)
                        : GetSpeedCoeffVelocity(xNormalized, yNormalized, sqDist);

  velocity = GetTanEffectVelocity(sqDist, velocity);

  return GetRotatedVelocity(velocity);
}

inline auto FilterZoomVector::GetSpeedCoeffVelocity(const float xNormalized,
                                                    const float yNormalized,
                                                    const float sqDist) const -> V2dFlt
{
  const float speedCoeff = GetSpeedCoefficient(xNormalized, yNormalized, sqDist);

  return {speedCoeff * xNormalized, speedCoeff * yNormalized};
}

inline auto FilterZoomVector::GetImageDisplacementVelocity(const float xNormalized,
                                                           const float yNormalized) const -> V2dFlt
{
  if (m_filterSettings->imageDisplacement == nullptr)
  {
    throw std::logic_error("No image displacement map setup.");
  }

  const V2dFlt displacement =
      m_filterSettings->imageDisplacement->GetDisplacementVector({xNormalized, yNormalized});
  return m_filterSettings->imageDisplacementAmplitude * displacement;
}

auto FilterZoomVector::GetSpeedCoefficient([[maybe_unused]] const float xNormalized,
                                           const float yNormalized,
                                           const float sqDist) const -> float
{
  float speedCoeff = (1.0F + m_filterSettings->vitesse.GetRelativeSpeed()) /
                     ZoomFilterData::SPEED_COEFF_DENOMINATOR;

  switch (m_filterSettings->mode)
  {
    case ZoomFilterMode::AMULET_MODE:
    {
      speedCoeff += m_filterSettings->amuletAmplitude * sqDist;
      break;
    }
    case ZoomFilterMode::CRYSTAL_BALL_MODE0:
    case ZoomFilterMode::CRYSTAL_BALL_MODE1:
    {
      speedCoeff -= m_filterSettings->crystalBallAmplitude *
                    (sqDist - m_filterSettings->crystalBallSqDistOffset);
      break;
    }
    case ZoomFilterMode::HYPERCOS_MODE0:
    case ZoomFilterMode::HYPERCOS_MODE1:
    case ZoomFilterMode::HYPERCOS_MODE2:
    case ZoomFilterMode::IMAGE_DISPLACEMENT_MODE:
    case ZoomFilterMode::NORMAL_MODE:
    {
      break;
    }
    case ZoomFilterMode::SCRUNCH_MODE:
    {
      speedCoeff += m_filterSettings->scrunchAmplitude * sqDist;
      break;
    }
    case ZoomFilterMode::SPEEDWAY_MODE:
    {
      constexpr float SQ_DIST_FACTOR = 0.01;
      speedCoeff *= m_filterSettings->speedwayAmplitude * (yNormalized + SQ_DIST_FACTOR * sqDist);
      break;
    }
    case ZoomFilterMode::WAVE_MODE0:
    case ZoomFilterMode::WAVE_MODE1:
    {
      speedCoeff += GetWaveEffectSpeedCoeff(sqDist);
      break;
    }
    case ZoomFilterMode::Y_ONLY_MODE:
    {
      speedCoeff *= GetYOnlySpeedCoeff(xNormalized, yNormalized);
      break;
    }
      /* Amulette 2 */
      // vx = X * tan(dist);
      // vy = Y * tan(dist);
    default:
      throw std::logic_error(std20::format("Switch: unhandled case '{}'.", m_filterSettings->mode));
  }

  return GetClampedSpeedCoeff(speedCoeff);
}

auto FilterZoomVector::GetWaveEffectSpeedCoeff(const float sqDist) const -> float
{
  const float angle = m_filterSettings->waveFreqFactor * sqDist;
  float periodicPart;
  switch (m_filterSettings->waveEffectType)
  {
    case ZoomFilterData::WaveEffect::WAVE_SIN_EFFECT:
      periodicPart = std::sin(angle);
      break;
    case ZoomFilterData::WaveEffect::WAVE_COS_EFFECT:
      periodicPart = std::cos(angle);
      break;
    case ZoomFilterData::WaveEffect::WAVE_SIN_COS_EFFECT:
      periodicPart = std::sin(angle) + std::cos(angle);
      break;
    default:
      throw std::logic_error("Unknown WaveEffect enum");
  }
  return m_filterSettings->waveAmplitude * periodicPart;
}

auto FilterZoomVector::GetYOnlySpeedCoeff([[maybe_unused]] const float xNormalized,
                                          const float yNormalized) const -> float
{
  switch (m_filterSettings->yOnlyEffect)
  {
    case ZoomFilterData::YOnlyEffect::XSIN_YSIN:
      return m_filterSettings->yOnlyAmplitude *
             std::sin(m_filterSettings->yOnlyXFreqFactor * xNormalized) *
             std::sin(m_filterSettings->yOnlyFreqFactor * yNormalized);
    case ZoomFilterData::YOnlyEffect::XSIN_YCOS:
      return m_filterSettings->yOnlyAmplitude *
             std::sin(m_filterSettings->yOnlyXFreqFactor * xNormalized) *
             std::cos(m_filterSettings->yOnlyFreqFactor * yNormalized);
    case ZoomFilterData::YOnlyEffect::XCOS_YSIN:
      return m_filterSettings->yOnlyAmplitude *
             std::cos(m_filterSettings->yOnlyXFreqFactor * xNormalized) *
             std::sin(m_filterSettings->yOnlyFreqFactor * yNormalized);
    case ZoomFilterData::YOnlyEffect::XCOS_YCOS:
      return m_filterSettings->yOnlyAmplitude *
             std::cos(m_filterSettings->yOnlyXFreqFactor * xNormalized) *
             std::cos(m_filterSettings->yOnlyFreqFactor * yNormalized);
    default:
      throw std::logic_error(
          std20::format("Switch: unhandled case '{}'.", m_filterSettings->yOnlyEffect));
  }
}

auto FilterZoomVector::GetClampedSpeedCoeff(const float speedCoeff) const -> float
{
  if (speedCoeff < ZoomFilterData::MIN_SPEED_COEFF)
  {
    UpdateDoZoomVectorSpeedCoeffBelowMinStats();
    return ZoomFilterData::MIN_SPEED_COEFF;
  }
  if (speedCoeff > m_maxSpeedCoeff)
  {
    UpdateDoZoomVectorSpeedCoeffAboveMaxStats();
    return m_maxSpeedCoeff;
  }
  return speedCoeff;
}

inline auto FilterZoomVector::GetHypercosVelocity(const float xNormalized,
                                                  const float yNormalized) const -> V2dFlt
{
  const float hypercosFreqX = m_filterSettings->hypercosReverse ? -m_filterSettings->hypercosFreqX
                                                                : +m_filterSettings->hypercosFreqX;
  const float hypercosFreqY = m_filterSettings->hypercosReverse ? -m_filterSettings->hypercosFreqY
                                                                : +m_filterSettings->hypercosFreqY;
  float xVal = 0.0;
  float yVal = 0.0;

  switch (m_filterSettings->hypercosEffect)
  {
    case ZoomFilterData::HypercosEffect::NONE:
      break;
    case ZoomFilterData::HypercosEffect::SIN_RECTANGULAR:
      xVal = std::sin(hypercosFreqX * xNormalized);
      yVal = std::sin(hypercosFreqY * yNormalized);
      break;
    case ZoomFilterData::HypercosEffect::COS_RECTANGULAR:
      xVal = std::cos(hypercosFreqX * xNormalized);
      yVal = std::cos(hypercosFreqY * yNormalized);
      break;
    case ZoomFilterData::HypercosEffect::SIN_CURL_SWIRL:
      xVal = std::sin(hypercosFreqY * yNormalized);
      yVal = std::sin(hypercosFreqX * xNormalized);
      break;
    case ZoomFilterData::HypercosEffect::COS_CURL_SWIRL:
      xVal = std::cos(hypercosFreqY * yNormalized);
      yVal = std::cos(hypercosFreqX * xNormalized);
      break;
    case ZoomFilterData::HypercosEffect::SIN_COS_CURL_SWIRL:
      xVal = std::sin(hypercosFreqX * yNormalized);
      yVal = std::cos(hypercosFreqY * xNormalized);
      break;
    case ZoomFilterData::HypercosEffect::COS_SIN_CURL_SWIRL:
      xVal = std::cos(hypercosFreqY * yNormalized);
      yVal = std::sin(hypercosFreqX * xNormalized);
      break;
    case ZoomFilterData::HypercosEffect::SIN_TAN_CURL_SWIRL:
      xVal = std::sin(std::tan(hypercosFreqY * yNormalized));
      yVal = std::cos(std::tan(hypercosFreqX * xNormalized));
      break;
    case ZoomFilterData::HypercosEffect::COS_TAN_CURL_SWIRL:
      xVal = std::cos(std::tan(hypercosFreqY * yNormalized));
      yVal = std::sin(std::tan(hypercosFreqX * xNormalized));
      break;
    default:
      throw std::logic_error("Unknown filterData.hypercosEffect value");
  }

  //  xVal = stdnew::clamp(std::tan(hypercosFreqY * xVal), -1.0, 1.0);
  //  yVal = stdnew::clamp(std::tan(hypercosFreqX * yVal), -1.0, 1.0);

  return {m_filterSettings->hypercosAmplitudeX * xVal, m_filterSettings->hypercosAmplitudeY * yVal};
}

inline auto FilterZoomVector::GetHPlaneEffectVelocity([[maybe_unused]] const float xNormalized,
                                                      const float yNormalized) const -> float
{
  // TODO - try xNormalized
  return yNormalized * m_filterSettings->hPlaneEffectAmplitude *
         static_cast<float>(m_filterSettings->hPlaneEffect);
}

inline auto FilterZoomVector::GetVPlaneEffectVelocity(
    [[maybe_unused]] const float xNormalized, [[maybe_unused]] const float yNormalized) const
    -> float
{
  // TODO - try yNormalized
  return xNormalized * m_filterSettings->vPlaneEffectAmplitude *
         static_cast<float>(m_filterSettings->vPlaneEffect);
}

inline auto FilterZoomVector::GetNoiseVelocity() const -> V2dFlt
{
  if (m_filterSettings->noiseFactor < SMALL_FLOAT)
  {
    return {0.0, 0.0};
  }

  UpdateDoZoomVectorNoiseFactorStats();
  //    const float xAmp = 1.0/getRandInRange(50.0f, 200.0f);
  //    const float yAmp = 1.0/getRandInRange(50.0f, 200.0f);
  const float amp = 0.5F * m_filterSettings->noiseFactor /
                    GetRandInRange(ZoomFilterData::NOISE_MIN, ZoomFilterData::NOISE_MAX);
  return {GetRandInRange(-amp, +amp), GetRandInRange(-amp, +amp)};
}

inline auto FilterZoomVector::GetTanEffectVelocity(const float sqDist, const V2dFlt& velocity) const
    -> V2dFlt
{
  if (!m_filterSettings->tanEffect)
  {
    return velocity;
  }

  UpdateDoZoomVectorTanEffectStats();
  const float tanArg =
      stdnew::clamp(std::fmod(sqDist, m_half_pi), -0.85 * m_half_pi, 0.85 * m_half_pi);
  const float tanSqDist = std::tan(tanArg);
  return {tanSqDist * velocity.x, tanSqDist * velocity.y};
}

inline auto FilterZoomVector::GetRotatedVelocity(const V2dFlt& velocity) const -> V2dFlt
{
  if (std::fabs(m_filterSettings->rotateSpeed) < SMALL_FLOAT)
  {
    return velocity;
  }

  if (m_filterSettings->rotateSpeed < 0.0F)
  {
    UpdateDoZoomVectorNegativeRotateStats();
    return {-m_filterSettings->rotateSpeed * (velocity.x - velocity.y),
            -m_filterSettings->rotateSpeed * (velocity.x + velocity.y)};
  }

  UpdateDoZoomVectorPositiveRotateStats();
  return {m_filterSettings->rotateSpeed * (velocity.y + velocity.x),
          m_filterSettings->rotateSpeed * (velocity.y - velocity.x)};
}

inline void FilterZoomVector::UpdateDoZoomVectorStats() const
{
  if (m_stats == nullptr)
  {
    return;
  }
  m_stats->DoZoomVector();
}

inline void FilterZoomVector::UpdateDoZoomVectorNoisifyStats() const
{
  if (m_stats == nullptr)
  {
    return;
  }
  m_stats->DoZoomVectorNoisify();
}

inline void FilterZoomVector::UpdateDoZoomVectorHypercosEffectStats() const
{
  if (m_stats == nullptr)
  {
    return;
  }
  m_stats->DoZoomVectorHypercosEffect();
}

inline void FilterZoomVector::UpdateDoZoomVectorHPlaneEffectStats() const
{
  if (m_stats == nullptr)
  {
    return;
  }
  m_stats->DoZoomVectorHPlaneEffect();
}

inline void FilterZoomVector::UpdateDoZoomVectorVPlaneEffectStats() const
{
  if (m_stats == nullptr)
  {
    return;
  }
  m_stats->DoZoomVectorVPlaneEffect();
}

inline void FilterZoomVector::UpdateDoZoomVectorSpeedCoeffBelowMinStats() const
{
  if (m_stats == nullptr)
  {
    return;
  }
  m_stats->DoZoomVectorSpeedCoeffBelowMin();
}

inline void FilterZoomVector::UpdateDoZoomVectorSpeedCoeffAboveMaxStats() const
{
  if (m_stats == nullptr)
  {
    return;
  }
  m_stats->DoZoomVectorSpeedCoeffAboveMax();
}

inline void FilterZoomVector::UpdateDoZoomVectorNoiseFactorStats() const
{
  if (m_stats == nullptr)
  {
    return;
  }
  m_stats->DoZoomVectorNoiseFactor();
}

inline void FilterZoomVector::UpdateDoZoomVectorTanEffectStats() const
{
  if (m_stats == nullptr)
  {
    return;
  }
  m_stats->DoZoomVectorTanEffect();
}

inline void FilterZoomVector::UpdateDoZoomVectorNegativeRotateStats() const
{
  if (m_stats == nullptr)
  {
    return;
  }
  m_stats->DoZoomVectorNegativeRotate();
}

inline void FilterZoomVector::UpdateDoZoomVectorPositiveRotateStats() const
{
  if (m_stats == nullptr)
  {
    return;
  }
  m_stats->DoZoomVectorPositiveRotate();
}

} // namespace GOOM
