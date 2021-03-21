#include "tentacles.h"

#include "goom_graphic.h"
#include "goomutils/colormaps.h"
#include "goomutils/colorutils.h"
#include "goomutils/logging_control.h"
//#undef NO_LOGGING
#include "goomutils/logging.h"
#include "goomutils/mathutils.h"

#include <cmath>
#include <format>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace GOOM
{

#if __cplusplus <= 201402L
const size_t Tentacle2D::MIN_NUM_NODES = 10;
#endif

using namespace GOOM::UTILS;

Tentacle2D::Tentacle2D(const size_t id,
                       const size_t numNodes,
                       const double xMin,
                       const double xMax,
                       const double yMin,
                       const double yMax,
                       const double basePrevYWeight,
                       const double baseCurrentYWeight) noexcept
  : m_id{id},
    m_numNodes{numNodes},
    m_xMin{xMin},
    m_xMax{xMax},
    m_yMin{yMin},
    m_yMax{yMax},
    m_basePrevYWeight{basePrevYWeight},
    m_baseCurrentYWeight{baseCurrentYWeight},
    m_dampingFunc{CreateDampingFunc(m_basePrevYWeight, m_xMin, m_xMax)}
{
}

void Tentacle2D::SetXDimensions(const double x0, const double x1)
{
  if (m_startedIterating)
  {
    throw std::runtime_error("Can't set x dimensions after iteration start.");
  }

  m_xMin = x0;
  m_xMax = x1;
  ValidateXDimensions();

  m_dampingFunc = CreateDampingFunc(m_basePrevYWeight, m_xMin, m_xMax);
}

void Tentacle2D::ValidateSettings() const
{
  ValidateXDimensions();
  ValidateYDimensions();
  ValidateNumNodes();
  ValidatePrevYWeight();
  ValidateCurrentYWeight();
}

void Tentacle2D::ValidateXDimensions() const
{
  if (m_xMax <= m_xMin)
  {
    throw std::logic_error(std20::format("xmax must be > xmin, not ({}, {}).", m_xMin, m_xMax));
  }
}

void Tentacle2D::ValidateYDimensions() const
{
  if (m_yMax <= m_yMin)
  {
    throw std::logic_error(std20::format("ymax must be > ymin, not ({}, {}).", m_yMin, m_yMax));
  }
}

void Tentacle2D::ValidateNumNodes() const
{
  if (m_numNodes < MIN_NUM_NODES)
  {
    throw std::runtime_error(
        std20::format("numNodes must be >= {}, not {}.", MIN_NUM_NODES, m_numNodes));
  }
}

void Tentacle2D::ValidatePrevYWeight() const
{
  if (m_basePrevYWeight < 0.001)
  {
    throw std::runtime_error(
        std20::format("prevYWeight must be >= 0.001, not {}.", m_basePrevYWeight));
  }
}

void Tentacle2D::ValidateCurrentYWeight() const
{
  if (m_baseCurrentYWeight < 0.001)
  {
    throw std::runtime_error(
        std20::format("currentYWeight must be >= 0.001, not {}.", m_baseCurrentYWeight));
  }
}

inline auto Tentacle2D::Damp(const size_t nodeNum) const -> double
{
  return m_dampingCache[nodeNum];
}

void Tentacle2D::IterateNTimes(const size_t n)
{
  StartIterating();
  for (size_t i = 0; i < n; i++)
  {
    Iterate();
  }
  FinishIterating();
}

void Tentacle2D::StartIterating()
{
  ValidateSettings();

  m_startedIterating = true;
  m_iterNum = 0;

  const double xStep = (m_xMax - m_xMin) / static_cast<double>(m_numNodes - 1);
  const double yStep =
      100.0 /
      static_cast<double>(m_numNodes - 1); // ?????????????????????????????????????????????????????

  m_xVec.resize(m_numNodes);
  m_yVec.resize(m_numNodes);
  m_dampedYVec.resize(m_numNodes);
  m_dampingCache.resize(m_numNodes);
  double x = m_xMin;
  double y = m_yMin;
  for (size_t i = 0; i < m_numNodes; ++i)
  {
    m_dampingCache[i] = GetDamping(x);
    m_xVec[i] = x;
    m_yVec[i] = 0.1 * m_dampingCache[i];

    x += xStep;
    y += yStep;
  }
}

void Tentacle2D::Iterate()
{
  m_iterNum++;

  m_yVec[0] = GetFirstY();
  for (size_t i = 1; i < m_numNodes; i++)
  {
    m_yVec[i] = GetNextY(i);
  }

  UpdateDampedVals(m_yVec);
}

void Tentacle2D::UpdateDampedVals(const std::vector<double>& yVals)
{
  constexpr size_t NUM_SMOOTH_NODES = std::min(10UL, MIN_NUM_NODES);
  const auto tSmooth = [](const double t) { return t * (2.0 - t); };

  const double tStep = 1.0 / (NUM_SMOOTH_NODES - 1);
  double tNext = tStep;
  m_dampedYVec[0] = 0.0;
  for (size_t i = 1; i < NUM_SMOOTH_NODES; i++)
  {
    const double t = tSmooth(tNext);
    m_dampedYVec[i] =
        stdnew::lerp(m_dampedYVec[i - 1], static_cast<double>(GetDampedVal(i, yVals[i])), t);
    tNext += tStep;
  }

  for (size_t i = NUM_SMOOTH_NODES; i < m_numNodes; i++)
  {
    m_dampedYVec[i] = GetDampedVal(i, yVals[i]);
  }
}

auto Tentacle2D::GetXAndYVectors() const -> const Tentacle2D::XAndYVectors&
{
  if (std::get<0>(m_vecs).size() < 2)
  {
    throw std::runtime_error(std20::format("GetXAndYVectors: xvec.size() must be >= 2, not {}.",
                                           std::get<0>(m_vecs).size()));
  }
  if (m_xVec.size() < 2)
  {
    throw std::runtime_error(
        std20::format("GetXAndYVectors: xvec.size() must be >= 2, not {}.", m_xVec.size()));
  }
  if (m_yVec.size() < 2)
  {
    throw std::runtime_error(
        std20::format("GetXAndYVectors: yvec.size() must be >= 2, not {}.", m_yVec.size()));
  }

  return m_vecs;
}

inline auto Tentacle2D::GetFirstY() -> float
{
  return static_cast<float>(stdnew::lerp(m_yVec[0], m_iterZeroYVal, m_iterZeroLerpFactor));
}

inline auto Tentacle2D::GetNextY(const size_t nodeNum) -> float
{
  const double prevY = m_yVec[nodeNum - 1];
  const double currentY = m_yVec[nodeNum];

  return static_cast<float>(m_basePrevYWeight * prevY + m_baseCurrentYWeight * currentY);
}

inline auto Tentacle2D::GetDampedVal(const size_t nodeNum, const double val) const -> double
{
  if (!m_doDamping)
  {
    return val;
  }
  return Damp(nodeNum) * val;
}

auto Tentacle2D::GetDampedXAndYVectors() const -> const Tentacle2D::XAndYVectors&
{
  if (m_xVec.size() < 2)
  {
    throw std::runtime_error(
        std20::format("GetDampedXAndYVectors: xvec.size() must be >= 2, not {}.", m_xVec.size()));
  }
  if (m_dampedYVec.size() < 2)
  {
    throw std::runtime_error(std20::format(
        "GetDampedXAndYVectors: yvec.size() must be >= 2, not {}.", m_dampedYVec.size()));
  }
  if (std::get<0>(m_vecs).size() < 2)
  {
    throw std::runtime_error(
        std20::format("GetDampedXAndYVectors: std::get<0>(vecs).size() must be >= 2, not {}.",
                      std::get<0>(m_vecs).size()));
  }

  return m_dampedVecs;
}

auto Tentacle2D::CreateDampingFunc(const double prevYWeight, const double xMin, const double xMax)
    -> Tentacle2D::DampingFuncPtr
{
  if (prevYWeight < 0.6)
  {
    return CreateLinearDampingFunc(xMin, xMax);
  }
  return CreateExpDampingFunc(xMin, xMax);
}

auto Tentacle2D::CreateExpDampingFunc(const double xMin, const double xMax)
    -> Tentacle2D::DampingFuncPtr
{
  const double xRiseStart = xMin + 0.25 * xMax;
  constexpr double DAMP_START = 5.0;
  constexpr double DAMP_MAX = 30.0;

  return DampingFuncPtr{new ExpDampingFunction{0.1, xRiseStart, DAMP_START, xMax, DAMP_MAX}};
}

auto Tentacle2D::CreateLinearDampingFunc(const double xMin, const double xMax)
    -> Tentacle2D::DampingFuncPtr
{
  constexpr float Y_SCALE = 30.0;

  std::vector<std::tuple<double, double, DampingFuncPtr>> pieces{};
  (void)pieces.emplace_back(
      std::make_tuple(xMin, 0.1 * xMax, DampingFuncPtr{new FlatDampingFunction{0.1}}));
  (void)pieces.emplace_back(
      std::make_tuple(0.1 * xMax, 10 * xMax,
                      DampingFuncPtr{new LinearDampingFunction{0.1 * xMax, 0.1, xMax, Y_SCALE}}));

  return DampingFuncPtr{new PiecewiseDampingFunction{pieces}};
}

Tentacle3D::Tentacle3D(std::unique_ptr<Tentacle2D> t,
                       const Pixel& headCol,
                       const Pixel& headColLow,
                       const V3dFlt& h,
                       const size_t numHdNodes) noexcept
  : m_tentacle{std::move(t)},
    m_headColor{headCol},
    m_headColorLow{headColLow},
    m_head{h},
    m_numHeadNodes{numHdNodes}
{
}

Tentacle3D::Tentacle3D(std::unique_ptr<Tentacle2D> t,
                       std::shared_ptr<const ITentacleColorizer> col,
                       const Pixel& headCol,
                       const Pixel& headColLow,
                       const V3dFlt& h,
                       const size_t numHdNodes) noexcept
  : m_tentacle{std::move(t)},
    m_colorizer{std::move(col)},
    m_headColor{headCol},
    m_headColorLow{headColLow},
    m_head{h},
    m_numHeadNodes{numHdNodes}
{
}

Tentacle3D::Tentacle3D(Tentacle3D&& o) noexcept
  : m_tentacle{std::move(o.m_tentacle)},
    m_colorizer{std::move(o.m_colorizer)},
    m_headColor{o.m_headColor},
    m_headColorLow{o.m_headColorLow},
    m_head{o.m_head},
    m_numHeadNodes{o.m_numHeadNodes}
{
}

auto Tentacle3D::GetColor(const size_t nodeNum) const -> Pixel
{
  return m_colorizer->GetColor(nodeNum);
}

auto Tentacle3D::GetMixedColors(const size_t nodeNum,
                                const Pixel& color,
                                const Pixel& colorLow) const -> std::tuple<Pixel, Pixel>
{
  if (nodeNum < GetNumHeadNodes())
  {
    // Color the tentacle head
    const float t =
        0.5F * (1.0F + static_cast<float>(nodeNum + 1) / static_cast<float>(GetNumHeadNodes() + 1));
    const Pixel mixedHeadColor = IColorMap::GetColorMix(m_headColor, color, t);
    const Pixel mixedHeadColorLow = IColorMap::GetColorMix(m_headColorLow, colorLow, t);
    return std::make_tuple(mixedHeadColor, mixedHeadColorLow);
  }

  float t = static_cast<float>(nodeNum + 1) / static_cast<float>(Get2DTentacle().GetNumNodes());
  if (m_reverseColorMix)
  {
    t = 1 - t;
  }

  const Pixel segmentColor = GetColor(nodeNum);
  const Pixel mixedColor = IColorMap::GetColorMix(color, segmentColor, t);
  const Pixel mixedColorLow = IColorMap::GetColorMix(colorLow, segmentColor, t);

  if (std::abs(GetHead().x) < 10)
  {
    const float brightnessCut = t * t;
    return std::make_tuple(GetBrighterColor(brightnessCut, mixedColor, true),
                           GetBrighterColor(brightnessCut, mixedColorLow, true));
  }

  return std::make_tuple(mixedColor, mixedColorLow);
}

auto Tentacle3D::GetMixedColors(const size_t nodeNum,
                                const Pixel& color,
                                const Pixel& colorLow,
                                const float brightness) const -> std::tuple<Pixel, Pixel>
{
  if (nodeNum < GetNumHeadNodes())
  {
    return GetMixedColors(nodeNum, color, colorLow);
  }

#if __cplusplus <= 201402L
  const auto mixedColors = GetMixedColors(nodeNum, color, colorLow);
  const auto mixedColor = std::get<0>(mixedColors);
  const auto mixedColorLow = std::get<1>(mixedColors);
#else
  const auto [mixedColor, mixedColorLow] = GetMixedColors(nodeNum, color, colorLow);
#endif
  const Pixel mixedColorPixel = mixedColor;
  const Pixel mixedColorLowPixel = mixedColorLow;
  //constexpr float gamma = 4.2;
  //const float brightnessWithGamma = std::pow(brightness, 1.0F / gamma);
  const float brightnessWithGamma = brightness;
  //  const float brightnessWithGamma =
  //      std::max(0.4F, std::pow(brightness * getLuma(color), 1.0F / gamma));
  //  const float brightnessWithGammaLow =
  //      std::max(0.4F, std::pow(brightness * getLuma(colorLow), 1.0F / gamma));
  return std::make_tuple(
      GetBrighterColor(brightnessWithGamma, mixedColorPixel, m_allowOverexposed),
      GetBrighterColor(brightnessWithGamma, mixedColorLowPixel, m_allowOverexposed));
}

auto Tentacle3D::GetVertices() const -> std::vector<V3dFlt>
{
#if __cplusplus <= 201402L
  const auto xyvecs = m_tentacle->GetDampedXAndYVectors();
  const auto xvec2D = std::get<0>(xyvecs);
  const auto yvec2D = std::get<1>(xyvecs);
#else
  const auto [xvec2D, yvec2D] = m_tentacle->GetDampedXAndYVectors();
#endif
  const size_t n = xvec2D.size();

  //  logInfo("Tentacle: {}, head.x = {}, head.y = {}, head.z = {}", "x", head.x, head.y, head.z);

  std::vector<V3dFlt> vec3d(n);
  const auto x0 = static_cast<float>(m_head.x);
  const auto y0 = static_cast<float>(m_head.y - yvec2D[0]);
  const auto z0 = static_cast<float>(m_head.z - xvec2D[0]);
  float xStep = 0.0;
  if (std::abs(x0) < 10.0)
  {
    const float xn = 0.1 * x0;
    xStep = (x0 - xn) / static_cast<float>(n);
  }
  float x = x0;
  for (size_t i = 0; i < n; i++)
  {
    vec3d[i].x = x;
    vec3d[i].z = static_cast<float>(z0 + xvec2D[i]);
    vec3d[i].y = static_cast<float>(y0 + yvec2D[i]);

    x -= xStep;
  }

  return vec3d;
}

void Tentacles3D::AddTentacle(Tentacle3D&& t)
{
  (void)m_tentacles.emplace_back(std::move(t));
}

void Tentacles3D::SetAllowOverexposed(const bool val)
{
  for (auto& t : m_tentacles)
  {
    t.SetAllowOverexposed(val);
  }
}

} // namespace GOOM
