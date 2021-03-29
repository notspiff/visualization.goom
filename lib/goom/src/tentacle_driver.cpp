#include "tentacle_driver.h"

#include "goomutils/colormaps.h"
#include "goomutils/colorutils.h"
#include "goomutils/goomrand.h"
#include "goomutils/logging_control.h"
//#undef NO_LOGGING
#include "goomutils/logging.h"
#include "goomutils/mathutils.h"
#include "goomutils/random_colormaps.h"
#include "tentacles.h"
#include "v2d.h"

#include <cmath>
#include <cstdint>
#include <format>
#include <functional>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace GOOM
{

using namespace GOOM::UTILS;

inline auto ChangeCurrentColorMapEvent() -> bool
{
  return ProbabilityOfMInN(3, 5);
}

const size_t TentacleDriver::CHANGE_CURRENT_COLOR_MAP_GROUP_EVERY_N_UPDATES = 400;
const size_t TentacleDriver::CHANGE_TENTACLE_COLOR_MAP_EVERY_N_UPDATES = 100;

TentacleDriver::TentacleDriver(const uint32_t screenWidth, const uint32_t screenHeight) noexcept
  : m_screenWidth{screenWidth},
    m_screenHeight{screenHeight},
    m_draw{m_screenWidth, m_screenHeight},
    m_colorMaps{}
{
  const IterParamsGroup iter1 = {
      {100, 0.600, 1.0, {1.5, -10.0, +10.0, m_pi}, 100.0},
      {125, 0.700, 2.0, {1.0, -10.0, +10.0, 0.0}, 105.0},
  };
  const IterParamsGroup iter2 = {
      {125, 0.700, 0.5, {1.0, -10.0, +10.0, 0.0}, 100.0},
      {150, 0.800, 1.5, {1.5, -10.0, +10.0, m_pi}, 105.0},
  };
  const IterParamsGroup iter3 = {
      {150, 0.800, 1.5, {1.5, -10.0, +10.0, m_pi}, 100.0},
      {200, 0.900, 2.5, {1.0, -10.0, +10.0, 0.0}, 105.0},
  };
  /***
  const IterParamsGroup iter1 = {
      {100, 0.700, 1.0, {1.5, -10.0, +10.0, m_pi}, 100.0},
      {125, 0.800, 2.0, {1.0, -10.0, +10.0, 0.0}, 105.0},
  };
  const IterParamsGroup iter2 = {
      {125, 0.710, 0.5, {1.0, -10.0, +10.0, 0.0}, 100.0},
      {150, 0.810, 1.5, {1.5, -10.0, +10.0, m_pi}, 105.0},
  };
  const IterParamsGroup iter3 = {
      {150, 0.720, 1.5, {1.5, -10.0, +10.0, m_pi}, 100.0},
      {200, 0.820, 2.5, {1.0, -10.0, +10.0, 0.0}, 105.0},
  };
  ***/

  m_iterParamsGroups = {
      iter1,
      iter2,
      iter3,
  };

  LogDebug("Constructed TentacleDriver.");
}

auto TentacleDriver::GetBuffSettings() const -> const FXBuffSettings&
{
  return m_buffSettings;
}

void TentacleDriver::SetBuffSettings(const FXBuffSettings& settings)
{
  m_buffSettings = settings;
  m_draw.SetBuffIntensity(m_buffSettings.buffIntensity);
  m_draw.SetAllowOverexposed(m_buffSettings.allowOverexposed);
  m_tentacles.SetAllowOverexposed(m_buffSettings.allowOverexposed);
}

auto TentacleDriver::GetNumTentacles() const -> size_t
{
  return m_numTentacles;
}

auto TentacleDriver::GetColorMode() const -> TentacleDriver::ColorModes
{
  return m_colorMode;
}

void TentacleDriver::SetColorMode(ColorModes m)
{
  m_colorMode = m;
}

constexpr double TENT2D_XMIN = 0.0;
constexpr double TENT2D_YMIN = 0.065736;
constexpr double TENT2D_YMAX = 10000.0;

void TentacleDriver::Init(const ITentacleLayout& l)
{
  LogDebug("Starting driver Init.");

  m_numTentacles = l.GetNumPoints();
  LogDebug("numTentacles = {}.", m_numTentacles);

  m_tentacleParams.resize(m_numTentacles);

  constexpr V3dFlt INITIAL_HEAD_POS = {0, 0, 0};

  const size_t numInParamGroup = m_numTentacles / m_iterParamsGroups.size();
  const float tStep = 1.0F / static_cast<float>(numInParamGroup - 1);
  LogDebug("numInTentacleGroup = {}, tStep = {:.2f}.", numInParamGroup, tStep);

  const ColorMapGroup initialColorMapGroup = m_colorMaps.GetRandomGroup();

  size_t paramsIndex = 0;
  float t = 0.0;
  for (size_t i = 0; i < m_numTentacles; i++)
  {
    const IterParamsGroup paramsGrp = m_iterParamsGroups.at(paramsIndex);
    if (i % numInParamGroup == 0)
    {
      if (paramsIndex < m_iterParamsGroups.size() - 1)
      {
        paramsIndex++;
      }
      t = 0.0;
    }
    const IterationParams params = paramsGrp.GetNext(t);
    t += tStep;
    m_tentacleParams[i] = params;

    std::shared_ptr<TentacleColorMapColorizer> colorizer{
        new TentacleColorMapColorizer{initialColorMapGroup, params.numNodes}};
    m_colorizers.push_back(std::move(colorizer));

    std::unique_ptr<Tentacle2D> tentacle2D{CreateNewTentacle2D(i, params)};
    LogDebug("Created tentacle2D {}.", i);

    // To hide the annoying flapping tentacle head, make near the head very dark.
    const Pixel headColor = GetIntColor(5, 5, 5);
    const Pixel headColorLow = headColor;
    Tentacle3D tentacle{std::move(tentacle2D),
                        m_colorizers[m_colorizers.size() - 1],
                        headColor,
                        headColorLow,
                        INITIAL_HEAD_POS,
                        Tentacle2D::MIN_NUM_NODES};

    m_tentacles.AddTentacle(std::move(tentacle));

    LogDebug("Added tentacle {}.", i);
  }

  UpdateTentaclesLayout(l);

  m_updateNum = 0;
}

auto TentacleDriver::IterParamsGroup::GetNext(const float t) const
    -> TentacleDriver::IterationParams
{
  const float prevYWeight =
      GetRandInRange(0.9F, 1.1F) *
      stdnew::lerp(static_cast<float>(first.prevYWeight), static_cast<float>(last.prevYWeight), t);
  IterationParams params{};
  params.length = GetRandInRange(1.0F, 1.1F * stdnew::lerp(static_cast<float>(first.length),
                                                           static_cast<float>(last.length), t));
  params.numNodes =
      size_t(GetRandInRange(0.9F, 1.1F) * stdnew::lerp(static_cast<float>(first.numNodes),
                                                       static_cast<float>(last.numNodes), t));
  params.prevYWeight = prevYWeight;
  params.iterZeroYValWave = first.iterZeroYValWave;
  params.iterZeroYValWaveFreq =
      GetRandInRange(0.9F, 1.1F) * stdnew::lerp(static_cast<float>(first.iterZeroYValWaveFreq),
                                                static_cast<float>(last.iterZeroYValWaveFreq), t);
  return params;
}

auto TentacleDriver::CreateNewTentacle2D(size_t id, const IterationParams& p)
    -> std::unique_ptr<Tentacle2D>
{
  LogDebug("Creating new tentacle2D {}...", id);

  const auto tentacleLen =
      static_cast<size_t>(GetRandInRange(0.99F, 1.01F) * static_cast<float>(p.length));
  //const size_t tentacleLen = params.length;
  const double tent2d_xmax = TENT2D_XMIN + static_cast<double>(tentacleLen);

  std::unique_ptr<Tentacle2D> tentacle{new Tentacle2D{
      id, p.numNodes,
      //  size_t(static_cast<float>(params.numNodes) * getRandInRange(0.9f, 1.1f))
      TENT2D_XMIN, tent2d_xmax, TENT2D_YMIN, TENT2D_YMAX, p.prevYWeight, 1.0 - p.prevYWeight}};
  LogDebug("Created new tentacle2D {}.", id);

  LogDebug("tentacle {:3}:"
           " tentacleLen = {:4}, tent2d_xmin = {:7.2f}, tent2d_xmax = {:5.2f},"
           " prevYWeight = {:5.2f}, curYWeight = {:5.2f}, numNodes = {:5}",
           id, tentacleLen, tent2d_xmin, tent2d_xmax, tentacle->GetPrevYWeight(),
           tentacle->GetCurrentYWeight(), tentacle->GetNumNodes());

  tentacle->SetDoDamping(true);

  return tentacle;
}

void TentacleDriver::StartIterating()
{
  for (auto& t : m_tentacles)
  {
    t.Get2DTentacle().StartIterating();
  }
}

[[maybe_unused]] void TentacleDriver::StopIterating()
{
  for (auto& t : m_tentacles)
  {
    t.Get2DTentacle().FinishIterating();
  }
}

void TentacleDriver::UpdateTentaclesLayout(const ITentacleLayout& l)
{
  LogDebug("Updating tentacles layout. numTentacles = {}.", m_numTentacles);
  assert(l.GetNumPoints() == m_numTentacles);

  std::vector<size_t> sortedLongestFirst(m_numTentacles);
  std::iota(sortedLongestFirst.begin(), sortedLongestFirst.end(), 0);
  const auto compareByLength = [this](const size_t id1, const size_t id2) -> bool {
    const double len1 = m_tentacles[id1].Get2DTentacle().GetLength();
    const double len2 = m_tentacles[id2].Get2DTentacle().GetLength();
    // Sort by longest first.
    return len1 > len2;
  };
  std::sort(sortedLongestFirst.begin(), sortedLongestFirst.end(), compareByLength);

  for (size_t i = 0; i < m_numTentacles; i++)
  {
    LogDebug("{} {} tentacle[{}].len = {:.2}.", i, sortedLongestFirst.at(i),
             sortedLongestFirst.at(i),
             m_tentacles[sortedLongestFirst.at(i)].Get2DTentacle().GetLength());
  }

  for (size_t i = 0; i < m_numTentacles; i++)
  {
    m_tentacles[sortedLongestFirst.at(i)].SetHead(l.GetPoints().at(i));
  }

  // To help with perspective, any tentacles near vertical centre will be shortened.
  for (auto& tentacle : m_tentacles)
  {
    const V3dFlt& head = tentacle.GetHead();
    if (std::fabs(head.x) < 10.0F)
    {
      Tentacle2D& tentacle2D = tentacle.Get2DTentacle();
      const double xmin = tentacle2D.GetXMin();
      const double xmax = tentacle2D.GetXMax();
      const double newXmax = xmin + 1.0 * (xmax - xmin);
      tentacle2D.SetXDimensions(xmin, newXmax);
      tentacle.SetNumHeadNodes(
          std::max(6 * Tentacle2D::MIN_NUM_NODES, tentacle.Get2DTentacle().GetNumNodes() / 2));
    }
  }
}

void TentacleDriver::MultiplyIterZeroYValWaveFreq(const float val)
{
  for (size_t i = 0; i < m_numTentacles; i++)
  {
    const float newFreq = val * m_tentacleParams[i].iterZeroYValWaveFreq;
    m_tentacleParams[i].iterZeroYValWave.SetFrequency(newFreq);
  }
}

void TentacleDriver::SetReverseColorMix(const bool val)
{
  for (auto& t : m_tentacles)
  {
    t.SetReverseColorMix(val);
  }
}

void TentacleDriver::UpdateIterTimers()
{
  for (auto* t : m_iterTimers)
  {
    t->Next();
  }
}

auto TentacleDriver::GetNextColorMapGroups() const -> std::vector<ColorMapGroup>
{
  const size_t numDifferentGroups =
      (m_colorMode == ColorModes::minimal || m_colorMode == ColorModes::oneGroupForAll ||
       ProbabilityOfMInN(99, 100))
          ? 1
          : GetRandInRange(1U, std::min(size_t(5U), m_colorizers.size()));
  std::vector<ColorMapGroup> groups(numDifferentGroups);
  for (size_t i = 0; i < numDifferentGroups; i++)
  {
    groups[i] = m_colorMaps.GetRandomGroup();
  }

  std::vector<ColorMapGroup> nextColorMapGroups(m_colorizers.size());
  const size_t numPerGroup = nextColorMapGroups.size() / numDifferentGroups;
  size_t n = 0;
  for (size_t i = 0; i < nextColorMapGroups.size(); i++)
  {
    nextColorMapGroups[i] = groups[n];
    if ((i % numPerGroup == 0) && (n < numDifferentGroups - 1))
    {
      n++;
    }
  }

  if (ProbabilityOfMInN(1, 2))
  {
    std::random_shuffle(nextColorMapGroups.begin(), nextColorMapGroups.end());
  }

  return nextColorMapGroups;
}

void TentacleDriver::CheckForTimerEvents()
{
  //  logDebug("Update num = {}: checkForTimerEvents", updateNum);

  if (m_updateNum % CHANGE_CURRENT_COLOR_MAP_GROUP_EVERY_N_UPDATES == 0)
  {
    const std::vector<ColorMapGroup> nextGroups = GetNextColorMapGroups();
    for (size_t i = 0; i < m_colorizers.size(); i++)
    {
      m_colorizers[i]->SetColorMapGroup(nextGroups[i]);
    }
    if (m_colorMode != ColorModes::minimal)
    {
      for (auto& colorizer : m_colorizers)
      {
        if (ChangeCurrentColorMapEvent())
        {
          colorizer->ChangeColorMap();
        }
      }
    }
  }
}

void TentacleDriver::FreshStart()
{
  const ColorMapGroup nextColorMapGroup = m_colorMaps.GetRandomGroup();
  for (auto& colorizer : m_colorizers)
  {
    colorizer->SetColorMapGroup(nextColorMapGroup);
    if (m_colorMode != ColorModes::minimal)
    {
      colorizer->ChangeColorMap();
    }
  }
}

void TentacleDriver::Update(float angle,
                            float distance,
                            float distance2,
                            const Pixel& color,
                            const Pixel& colorLow,
                            PixelBuffer& currentBuff,
                            PixelBuffer& nextBuff)
{
  m_updateNum++;
  LogInfo("Doing Update {}.", m_updateNum);

  UpdateIterTimers();
  CheckForTimerEvents();

  constexpr float ITER_ZERO_LERP_FACTOR = 0.9;

  for (size_t i = 0; i < m_numTentacles; i++)
  {
    Tentacle3D& tentacle = m_tentacles[i];
    Tentacle2D& tentacle2D = tentacle.Get2DTentacle();

    const float iterZeroYVal = m_tentacleParams[i].iterZeroYValWave.GetNext();
    tentacle2D.SetIterZeroLerpFactor(ITER_ZERO_LERP_FACTOR);
    tentacle2D.SetIterZeroYVal(iterZeroYVal);

    LogDebug("Starting Iterate {} for tentacle {}.", tentacle2D.GetIterNum() + 1,
             tentacle2D.GetID());
    tentacle2D.Iterate();

    LogDebug("Update num = {}, tentacle = {}, doing plot with angle = {}, "
             "distance = {}, distance2 = {}, color = {:x} and colorLow = {:x}.",
             m_updateNum, tentacle2D.GetID(), angle, distance, distance2, color.Rgba(),
             colorLow.Rgba());
    LogDebug("tentacle head = ({:.2f}, {:.2f}, {:.2f}).", tentacle.GetHead().x,
             tentacle.GetHead().y, tentacle.GetHead().z);

    Plot3D(tentacle, color, colorLow, angle, distance, distance2, currentBuff, nextBuff);
  }
}

inline auto GetBrightnessCut(const Tentacle3D& tentacle, const float distance2) -> float
{
  if (std::abs(tentacle.GetHead().x) < 10)
  {
    if (distance2 < 8)
    {
      return 0.5;
    }
    return 0.2;
  }
  return 1.0;
}

constexpr int COORD_IGNORE_VAL = -666;

void TentacleDriver::Plot3D(const Tentacle3D& tentacle,
                            const Pixel& dominantColor,
                            const Pixel& dominantColorLow,
                            float angle,
                            float distance,
                            float distance2,
                            PixelBuffer& currentBuff,
                            PixelBuffer& nextBuff)
{
  const std::vector<V3dFlt> vertices = tentacle.GetVertices();
  const size_t n = vertices.size();

  V3dFlt cam = {0.0, 0.0, -3.0}; // TODO ????????????????????????????????
  cam.z += distance2;
  cam.y += 2.0F * std::sin(-(angle - m_half_pi) / 4.3F);
  LogDebug("cam = ({:.2f}, {:.2f}, {:.2f}).", cam.x, cam.y, cam.z);

  float angleAboutY = angle;
  if (-10.0 < tentacle.GetHead().x && tentacle.GetHead().x < 0.0)
  {
    angleAboutY -= 0.05 * m_pi;
  }
  else if (0.0 <= tentacle.GetHead().x && tentacle.GetHead().x < 10.0)
  {
    angleAboutY += 0.05 * m_pi;
  }

  const float sina = std::sin(m_pi - angleAboutY);
  const float cosa = std::cos(m_pi - angleAboutY);
  LogDebug("angle = {:.2f}, angleAboutY = {:.2f}, sina = {:.2}, cosa = {:.2},"
           " distance = {:.2f}, distance2 = {:.2f}.",
           angle, angleAboutY, sina, cosa, distance, distance2);

  std::vector<V3dFlt> v3{vertices};
  for (size_t i = 0; i < n; i++)
  {
    LogDebug("v3[{}]  = ({:.2f}, {:.2f}, {:.2f}).", i, v3[i].x, v3[i].y, v3[i].z);
    RotateV3DAboutYAxis(sina, cosa, v3[i], v3[i]);
    TranslateV3D(cam, v3[i]);
    LogDebug("v3[{}]+ = ({:.2f}, {:.2f}, {:.2f}).", i, v3[i].x, v3[i].y, v3[i].z);
  }

  const std::vector<V2dInt> v2 = ProjectV3DOntoV2D(v3, distance);

  const float brightnessCut = GetBrightnessCut(tentacle, distance2);

  // Faraway tentacles get smaller and draw_line adds them together making them look
  // very white and over-exposed. If we reduce the brightness, then all the combined
  // tentacles look less bright and white and more colors show through.
  using GetMixedColorsFunc = std::function<std::tuple<Pixel, Pixel>(const size_t nodeNum)>;
  GetMixedColorsFunc getMixedColors = [&](const size_t nodeNum) {
    return tentacle.GetMixedColors(nodeNum, dominantColor, dominantColorLow, brightnessCut);
  };
  if (distance2 > 30.0)
  {
    const float randBrightness = GetRandInRange(0.1F, 0.2F);
    float brightness = std::max(randBrightness, 30.0F / distance2) * brightnessCut;
    getMixedColors = [&, brightness](const size_t nodeNum) {
      return tentacle.GetMixedColors(nodeNum, dominantColor, dominantColorLow, brightness);
    };
  }

  for (size_t nodeNum = 0; nodeNum < v2.size() - 1; nodeNum++)
  {
    const auto ix0 = static_cast<int>(v2[nodeNum].x);
    const auto ix1 = static_cast<int>(v2[nodeNum + 1].x);
    const auto iy0 = static_cast<int>(v2[nodeNum].y);
    const auto iy1 = static_cast<int>(v2[nodeNum + 1].y);

    if (((ix0 == COORD_IGNORE_VAL) && (iy0 == COORD_IGNORE_VAL)) ||
        ((ix1 == COORD_IGNORE_VAL) && (iy1 == COORD_IGNORE_VAL)))
    {
      LogDebug("Skipping draw ignore vals {}: ix0 = {}, iy0 = {}, ix1 = {}, iy1 = {}.", nodeNum,
               ix0, iy0, ix1, iy1);
    }
    else if ((ix0 == ix1) && (iy0 == iy1))
    {
      LogDebug("Skipping draw equal points {}: ix0 = {}, iy0 = {}, ix1 = {}, iy1 = {}.", nodeNum,
               ix0, iy0, ix1, iy1);
    }
    else
    {
      LogDebug("draw_line {}: ix0 = {}, iy0 = {}, ix1 = {}, iy1 = {}.", nodeNum, ix0, iy0, ix1,
               iy1);

#if __cplusplus <= 201402L
      const auto mixedColors = getMixedColors(nodeNum);
      const auto color = std::get<0>(mixedColors);
      const auto colorLow = std::get<1>(mixedColors);
#else
      const auto [color, colorLow] = getMixedColors(nodeNum);
#endif
      /**
auto [color, colorLow] = GetMixedColors(nodeNum);
if (-10 < tentacle.getHead().x && tentacle.GetHead().x < 0)
{
  color = Pixel::WHITE;
  colorLow = color;
}
else if (0 <= tentacle.getHead().x && tentacle.GetHead().x < 10)
{
  color = Pixel{0xFF00FF00};
  colorLow = color;
}
**/
      const std::vector<Pixel> colors{color, colorLow};

      LogDebug("draw_line {}: dominantColor = {:#x}, dominantColorLow = {:#x}.", nodeNum,
               dominantColor.Rgba(), dominantColorLow.Rgba());
      LogDebug("draw_line {}: color = {:#x}, colorLow = {:#x}, brightnessCut = {:.2f}.", nodeNum,
               color.Rgba(), colorLow.Rgba(), brightnessCut);

      // TODO buff right way around ??????????????????????????????????????????????????????????????
      const std::vector<PixelBuffer*> buffs{&currentBuff, &nextBuff};
      // TODO - Control brightness because of back buff??
      // One buff may be better????? Make lighten more aggressive over whole tentacle??
      // draw_line(frontBuff, ix0, iy0, ix1, iy1, color, 1280, 720);
      constexpr uint8_t THICKNESS = 1;
      m_draw.Line(buffs, ix0, iy0, ix1, iy1, colors, THICKNESS);
    }
  }
}

auto TentacleDriver::ProjectV3DOntoV2D(const std::vector<V3dFlt>& v3, float distance) const
    -> std::vector<V2dInt>
{
  std::vector<V2dInt> v2(v3.size());

  const int Xp0 =
      v3[0].ignore || (v3[0].z <= 2) ? 1 : static_cast<int>(distance * v3[0].x / v3[0].z);
  const int Xpn = v3[v3.size() - 1].ignore || (v3[v3.size() - 1].z <= 2)
                      ? 1
                      : static_cast<int>(distance * v3[v3.size() - 1].x / v3[v3.size() - 1].z);
  const float xSpread = std::min(1.0F, std::abs(static_cast<float>(Xp0 - Xpn)) / 10.0F);

  for (size_t i = 0; i < v3.size(); ++i)
  {
    if (!v3[i].ignore && (v3[i].z > 2))
    {
      const int Xp = static_cast<int>(xSpread * distance * v3[i].x / v3[i].z);
      const int Yp = static_cast<int>(xSpread * distance * v3[i].y / v3[i].z);
      v2[i].x = Xp + static_cast<int>(m_screenWidth >> 1);
      v2[i].y = -Yp + static_cast<int>(m_screenHeight >> 1);
      LogDebug("project_v3d_to_v2d i: {:3}: v3[].x = {:.2f}, v3[].y = {:.2f}, v2[].z = {:.2f}, Xp "
               "= {}, Yp = {}, v2[].x = {}, v2[].y = {}.",
               i, v3[i].x, v3[i].y, v3[i].z, Xp, Yp, v2[i].x, v2[i].y);
    }
    else
    {
      v2[i].x = v2[i].y = COORD_IGNORE_VAL;
      LogDebug("project_v3d_to_v2d i: {:3}: v3[i].x = {:.2f}, v3[i].y = {:.2f}, v2[i].z = {:.2f}, "
               "v2[i].x = {}, v2[i].y = {}.",
               i, v3[i].x, v3[i].y, v3[i].z, v2[i].x, v2[i].y);
    }
  }

  return v2;
}

inline void TentacleDriver::RotateV3DAboutYAxis(float sina,
                                                float cosa,
                                                const V3dFlt& vsrc,
                                                V3dFlt& vdest)
{
  const float vi_x = vsrc.x;
  const float vi_z = vsrc.z;
  vdest.x = vi_x * cosa - vi_z * sina;
  vdest.z = vi_x * sina + vi_z * cosa;
  vdest.y = vsrc.y;
}

inline void TentacleDriver::TranslateV3D(const V3dFlt& vadd, V3dFlt& vinOut)
{
  vinOut.x += vadd.x;
  vinOut.y += vadd.y;
  vinOut.z += vadd.z;
}

TentacleColorMapColorizer::TentacleColorMapColorizer(const ColorMapGroup cmg,
                                                     const size_t nNodes) noexcept
  : m_numNodes{nNodes},
    m_currentColorMapGroup{cmg},
    m_colorMap{m_colorMaps.GetRandomColorMapPtr(m_currentColorMapGroup, RandomColorMaps::ALL)},
    m_prevColorMap{m_colorMap}
{
}

auto TentacleColorMapColorizer::GetColorMapGroup() const -> ColorMapGroup
{
  return m_currentColorMapGroup;
}

void TentacleColorMapColorizer::SetColorMapGroup(ColorMapGroup c)
{
  m_currentColorMapGroup = c;
}

void TentacleColorMapColorizer::ChangeColorMap()
{
  // Save the current color map to do smooth transitions to next color map.
  m_prevColorMap = m_colorMap;
  m_tTransition = 1.0;
  m_colorMap = m_colorMaps.GetRandomColorMapPtr(m_currentColorMapGroup, RandomColorMaps::ALL);
}

auto TentacleColorMapColorizer::GetColor(size_t nodeNum) const -> Pixel
{
  const float t = static_cast<float>(nodeNum) / static_cast<float>(m_numNodes);
  Pixel nextColor = m_colorMap->GetColor(t);

  // Keep going with the smooth transition until tmix runs out.
  if (m_tTransition > 0.0)
  {
    nextColor = IColorMap::GetColorMix(nextColor, m_prevColorMap->GetColor(t), m_tTransition);
    m_tTransition -= TRANSITION_STEP;
  }

  return nextColor;
}

[[maybe_unused]] GridTentacleLayout::GridTentacleLayout(const float xmin,
                                                        const float xmax,
                                                        const size_t xNum,
                                                        const float ymin,
                                                        const float ymax,
                                                        const size_t yNum,
                                                        const float zConst)
  : m_points{}
{
  const float xStep = (xmax - xmin) / static_cast<float>(xNum - 1);
  const float yStep = (ymax - ymin) / static_cast<float>(yNum - 1);

  float y = ymin;
  for (size_t i = 0; i < yNum; i++)
  {
    float x = xmin;
    for (size_t j = 0; j < xNum; j++)
    {
      (void)m_points.emplace_back(V3dFlt{x, y, zConst});
      x += xStep;
    }
    y += yStep;
  }
}

auto GridTentacleLayout::GetNumPoints() const -> size_t
{
  return m_points.size();
}

auto GridTentacleLayout::GetPoints() const -> const std::vector<V3dFlt>&
{
  return m_points;
}

auto CirclesTentacleLayout::GetCircleSamples(const size_t numCircles,
                                             [[maybe_unused]] const size_t totalPoints)
    -> std::vector<size_t>
{
  std::vector<size_t> circleSamples(numCircles);

  return circleSamples;
}

CirclesTentacleLayout::CirclesTentacleLayout(const float radiusMin,
                                             const float radiusMax,
                                             const std::vector<size_t>& numCircleSamples,
                                             const float zConst)
  : m_points{}
{
  const size_t numCircles = numCircleSamples.size();
  if (numCircles < 2)
  {
    std::logic_error(std20::format("There must be >= 2 circle sample numbers not {}.", numCircles));
  }
  for (const auto numSample : numCircleSamples)
  {
    if (numSample % 2 != 0)
    {
      // Perspective looks bad with odd because of x=0 tentacle.
      std::logic_error(std20::format("Circle sample num must be even not {}.", numSample));
    }
  }

#ifndef NO_LOGGING
  // TODO - Should be lerps here?
  const auto logLastPoint = [&](size_t i, const float r, const float angle) {
    const size_t el = points.size() - 1;
    logDebug("  sample {:3}: angle = {:+6.2f}, cos(angle) = {:+6.2f}, r = {:+6.2f},"
             " pt[{:3}] = ({:+6.2f}, {:+6.2f}, {:+6.2f})",
             i, angle, cos(angle), r, el, points.at(el).x, points.at(el).y, points.at(el).z);
  };
#endif

  const auto getSamplePoints = [&](const float radius, const size_t numSample,
                                   const float angleStart, const float angleFinish) {
    const float angleStep = (angleFinish - angleStart) / static_cast<float>(numSample - 1);
    float angle = angleStart;
    for (size_t i = 0; i < numSample; i++)
    {
      const auto x = static_cast<float>(radius * std::cos(angle));
      const auto y = static_cast<float>(radius * std::sin(angle));
      const V3dFlt point = {x, y, zConst};
      m_points.push_back(point);
#ifndef NO_LOGGING
      logLastPoint(i, radius, angle);
#endif
      angle += angleStep;
    };
  };

  const float angleLeftStart = +m_half_pi;
  const float angleLeftFinish = 1.5 * m_pi;
  const float angleRightStart = -m_half_pi;
  const float angleRightFinish = +m_half_pi;
  LogDebug("Setup: angleLeftStart = {:.2f}, angleLeftFinish = {:.2f},"
           " angleRightStart = {:.2f}, angleRightFinish = {:.2f}",
           angleLeftStart, angleLeftFinish, angleRightStart, angleRightFinish);

  const float angleOffsetStart = 0.035 * m_pi;
  const float angleOffsetFinish = 0.035 * m_pi;
  const float offsetStep =
      (angleOffsetStart - angleOffsetFinish) / static_cast<float>(numCircles - 1);
  const float radiusStep = (radiusMax - radiusMin) / static_cast<float>(numCircles - 1);
  LogDebug("Setup: numCircles = {}, radiusStep = {:.2f}, radiusMin = {:.2f}, radiusMax = {:.2f},"
           " offsetStep = {:.2f}, angleOffsetStart = {:.2f}, angleOffsetFinish = {:.2f}",
           numCircles, radiusStep, radiusMin, radiusMax, offsetStep, angleOffsetStart,
           angleOffsetFinish);

  float r = radiusMax;
  float angleOffset = angleOffsetStart;
  for (const auto numSample : numCircleSamples)
  {
    LogDebug("Circle with {} samples: r = {:.2f}", numSample, r);

    getSamplePoints(r, numSample / 2, angleLeftStart + angleOffset, angleLeftFinish - angleOffset);
    getSamplePoints(r, numSample / 2, angleRightStart + angleOffset,
                    angleRightFinish - angleOffset);

    r -= radiusStep;
    angleOffset -= offsetStep;
  }
}

auto CirclesTentacleLayout::GetNumPoints() const -> size_t
{
  return m_points.size();
}

auto CirclesTentacleLayout::GetPoints() const -> const std::vector<V3dFlt>&
{
  return m_points;
}

} // namespace GOOM
