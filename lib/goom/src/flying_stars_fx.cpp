#include "flying_stars_fx.h"

#include "goom_config.h"
#include "goom_draw.h"
#include "goom_plugin_info.h"
#include "goom_stats.h"
#include "goom_visual_fx.h"
#include "goomutils/colormaps.h"
#include "goomutils/colorutils.h"
#include "goomutils/goomrand.h"
#include "goomutils/image_bitmaps.h"
//#include "goomutils/logging_control.h"
//#undef NO_LOGGING
//#include "goomutils/logging.h"
#include "goomutils/mathutils.h"
#include "goomutils/random_colormaps.h"
#include "goomutils/random_colormaps_manager.h"
#include "goomutils/small_image_bitmaps.h"
#include "stats/stars_stats.h"
#include "v2d.h"

#include <cmath>
#include <cstddef>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace GOOM
{

using namespace GOOM::UTILS;
using UTILS::COLOR_DATA::ColorMapName;

constexpr uint32_t MIN_STAR_AGE = 15;
constexpr uint32_t MAX_STAR_EXTRA_AGE = 50;

/* TODO:-- FAIRE PROPREMENT... BOAH... */

// The different modes of the visual FX.
enum class StarModes
{
  NO_FX = 0,
  FIREWORKS,
  RAIN,
  FOUNTAIN,
  _SIZE // must be last - gives number of enums
};
constexpr size_t NUM_FX = static_cast<uint32_t>(StarModes::_SIZE);

struct Star
{
  V2dFlt pos{};
  V2dFlt velocity{};
  V2dFlt acceleration{};
  float age = 0.0;
  float vage = 0.0;
  std::shared_ptr<const IColorMap> dominantColormap{};
  std::shared_ptr<const IColorMap> dominantLowColormap{};
  std::shared_ptr<const IColorMap> currentColorMap{};
  std::shared_ptr<const IColorMap> currentLowColorMap{};
};

class FlyingStarsFx::FlyingStarsImpl
{
public:
  explicit FlyingStarsImpl(std::shared_ptr<const PluginInfo> goomInfo) noexcept;
  ~FlyingStarsImpl() noexcept = default;
  FlyingStarsImpl(const FlyingStarsImpl&) noexcept = delete;
  FlyingStarsImpl(FlyingStarsImpl&&) noexcept = delete;
  auto operator=(const FlyingStarsImpl&) -> FlyingStarsImpl& = delete;
  auto operator=(FlyingStarsImpl&&) -> FlyingStarsImpl& = delete;

  [[nodiscard]] auto GetResourcesDirectory() const -> const std::string&;
  void SetResourcesDirectory(const std::string& dirName);
  void SetSmallImageBitmaps(const SmallImageBitmaps& smallBitmaps);
  void SetBuffSettings(const FXBuffSettings& settings);

  void Start();

  void UpdateBuffers(PixelBuffer& currentBuff, PixelBuffer& nextBuff);

  void Finish();
  void Log(const GoomStats::LogStatsValueFunc& logVal) const;

private:
  const std::shared_ptr<const PluginInfo> m_goomInfo;
  const int32_t m_halfWidth;
  const int32_t m_halfHeight;

  const std::shared_ptr<WeightedColorMaps> m_colorMaps{
      std::make_shared<WeightedColorMaps>(Weights<ColorMapGroup>{{
          {ColorMapGroup::PERCEPTUALLY_UNIFORM_SEQUENTIAL, 10},
          {ColorMapGroup::SEQUENTIAL, 10},
          {ColorMapGroup::SEQUENTIAL2, 10},
          {ColorMapGroup::CYCLIC, 0},
          {ColorMapGroup::DIVERGING, 0},
          {ColorMapGroup::DIVERGING_BLACK, 0},
          {ColorMapGroup::QUALITATIVE, 10},
          {ColorMapGroup::MISC, 10},
      }})};
  const std::shared_ptr<WeightedColorMaps> m_lowColorMaps{
      std::make_shared<WeightedColorMaps>(Weights<ColorMapGroup>{{
          {ColorMapGroup::PERCEPTUALLY_UNIFORM_SEQUENTIAL, 10},
          {ColorMapGroup::SEQUENTIAL, 0},
          {ColorMapGroup::SEQUENTIAL2, 10},
          {ColorMapGroup::CYCLIC, 5},
          {ColorMapGroup::DIVERGING, 10},
          {ColorMapGroup::DIVERGING_BLACK, 20},
          {ColorMapGroup::QUALITATIVE, 1},
          {ColorMapGroup::MISC, 10},
      }})};
  RandomColorMapsManager m_randomColorMapsManager{};
  uint32_t m_dominantColorMapID{};
  uint32_t m_dominantLowColorMapID{};
  uint32_t m_colorMapID{};
  uint32_t m_lowColorMapID{};

  ColorMode m_colorMode = ColorMode::MIX_COLORS;
  void ChangeColorMode();
  [[nodiscard]] auto GetMixedColors(const Star& s, float t, float brightness)
      -> std::tuple<Pixel, Pixel>;

  std::string m_resourcesDirectory{};

  uint32_t m_counter = 0;
  static constexpr uint32_t MAX_COUNT = 100;

  StarModes m_fxMode = StarModes::FIREWORKS;
  static constexpr uint32_t MAX_NUM_STARS = 1024;
  static constexpr uint32_t MIN_NUM_STARS = 100;
  uint32_t m_maxStars = MAX_NUM_STARS;
  std::vector<Star> m_stars{};
  static constexpr float OLD_AGE = 0.95;
  uint32_t m_maxStarAge = 15;

  static constexpr float MIN_MIN_SIDE_WIND = -0.10F;
  static constexpr float MAX_MIN_SIDE_WIND = -0.01F;
  static constexpr float MIN_MAX_SIDE_WIND = +0.01F;
  static constexpr float MAX_MAX_SIDE_WIND = +0.10F;
  float m_minSideWind = 0.0;
  float m_maxSideWind = 0.00001;

  static constexpr float MIN_MIN_GRAVITY = +0.005F;
  static constexpr float MAX_MIN_GRAVITY = +0.010F;
  static constexpr float MIN_MAX_GRAVITY = +0.050F;
  static constexpr float MAX_MAX_GRAVITY = +0.090F;
  float m_minGravity = MAX_MIN_GRAVITY;
  float m_maxGravity = MAX_MAX_GRAVITY;

  // Fireworks Largest Bombs
  float m_minAge = 1.0F - (99.0F / 100.0F);
  // Fireworks Smallest Bombs
  float m_maxAge = 1.0F - (80.0F / 100.0F);

  FXBuffSettings m_buffSettings{};
  bool m_useSingleBufferOnly = true;
  GoomDraw m_draw;
  StarsStats m_stats{};

  void CheckForStarEvents();
  void SoundEventOccurred();
  void UpdateWindAndGravity();
  void ChangeColorMaps();
  void UpdateStarColorMaps(float angle, Star& star);
  static constexpr size_t NUM_SEGMENTS = 20;
  std::array<COLOR_DATA::ColorMapName, NUM_SEGMENTS> m_angleColorMapName{};
  void UpdateAngleColorMapNames();
  static auto GetSegmentNum(float angle) -> size_t;
  auto GetDominantColorMapPtr(float angle) const -> std::shared_ptr<const IColorMap>;
  auto GetDominantLowColorMapPtr(float angle) const -> std::shared_ptr<const IColorMap>;
  auto GetCurrentColorMapPtr(float angle) const -> std::shared_ptr<const IColorMap>;
  auto GetCurrentLowColorMapPtr(float angle) const -> std::shared_ptr<const IColorMap>;
  void DrawStars(PixelBuffer& currentBuff, PixelBuffer& nextBuff);
  static void UpdateStar(Star& s);
  [[nodiscard]] auto IsStarDead(const Star& s) const -> bool;
  enum class DrawMode
  {
    CIRCLES,
    LINES,
    DOTS,
    CIRCLES_AND_LINES,
  };
  DrawMode m_drawMode = DrawMode::CIRCLES;
  void ChangeDrawMode();
  const SmallImageBitmaps* m_smallBitmaps{};
  auto GetImageBitmap(size_t size) -> const ImageBitmap&;
  using DrawFunc = std::function<void(const std::vector<PixelBuffer*>& buffs,
                                      int32_t x1,
                                      int32_t y1,
                                      int32_t x2,
                                      int32_t y2,
                                      uint32_t size,
                                      const std::vector<Pixel>& colors)>;
  const std::map<DrawMode, const DrawFunc> m_drawFuncs{};
  auto GetDrawFunc() const -> DrawFunc;
  void DrawParticle(PixelBuffer& currentBuff,
                    PixelBuffer& nextBuff,
                    const Star& star,
                    float flipSpeed,
                    const DrawFunc& drawFunc);
  void DrawParticleCircle(const std::vector<PixelBuffer*>& buffs,
                          int32_t x1,
                          int32_t y1,
                          int32_t x2,
                          int32_t y2,
                          uint32_t size,
                          const std::vector<Pixel>& colors);
  void DrawParticleLine(const std::vector<PixelBuffer*>& buffs,
                        int32_t x1,
                        int32_t y1,
                        int32_t x2,
                        int32_t y2,
                        uint32_t size,
                        const std::vector<Pixel>& colors);
  void DrawParticleDot(const std::vector<PixelBuffer*>& buffs,
                       int32_t x1,
                       int32_t y1,
                       int32_t x2,
                       int32_t y2,
                       uint32_t size,
                       const std::vector<Pixel>& colors);
  void RemoveDeadStars();

  struct StarModeParams
  {
    V2dInt pos{};
    float gravityFactor = 1.0F;
    float windFactor = 1.0F;
    float vage = 0.0;
    float radius = 1.0F;
  };
  auto GetStarParams(float defaultRadius, float heightRatio) -> StarModeParams;
  auto GetFireworksStarParams(float defaultRadius) const -> StarModeParams;
  auto GetRainStarParams(float defaultRadius) const -> StarModeParams;
  auto GetFountainStarParams(float defaultRadius) const -> StarModeParams;
  void AddStarBombs(const StarModeParams& starModeParams, size_t maxStarsInBomb);
  auto GetMaxStarsInABomb(const float heightRatio) const -> size_t;
  void AddABomb(const V2dInt& pos, float radius, float vage, float gravity, float sideWind);
  [[nodiscard]] auto GetBombAngle(const Star& star) const -> float;

  static constexpr size_t MIN_DOT_SIZE = 3;
  static constexpr size_t MAX_DOT_SIZE = 5;
  static_assert(MAX_DOT_SIZE <= SmallImageBitmaps::MAX_IMAGE_SIZE, "Max dot size mismatch.");
};

FlyingStarsFx::FlyingStarsFx(const std::shared_ptr<const PluginInfo>& info) noexcept
  : m_fxImpl{new FlyingStarsImpl{info}}
{
}

FlyingStarsFx::~FlyingStarsFx() noexcept = default;

auto FlyingStarsFx::GetResourcesDirectory() const -> const std::string&
{
  return m_fxImpl->GetResourcesDirectory();
}

void FlyingStarsFx::SetResourcesDirectory(const std::string& dirName)
{
  m_fxImpl->SetResourcesDirectory(dirName);
}

void FlyingStarsFx::SetBuffSettings(const FXBuffSettings& settings)
{
  m_fxImpl->SetBuffSettings(settings);
}

void FlyingStarsFx::SetSmallImageBitmaps(const SmallImageBitmaps& smallBitmaps)
{
  m_fxImpl->SetSmallImageBitmaps(smallBitmaps);
}

void FlyingStarsFx::Start()
{
  m_fxImpl->Start();
}

void FlyingStarsFx::Finish()
{
  m_fxImpl->Finish();
}

void FlyingStarsFx::Log(const GoomStats::LogStatsValueFunc& logVal) const
{
  m_fxImpl->Log(logVal);
}

auto FlyingStarsFx::GetFxName() const -> std::string
{
  return "Flying Stars FX";
}

void FlyingStarsFx::Apply(PixelBuffer& currentBuff, PixelBuffer& nextBuff)
{
  if (!m_enabled)
  {
    return;
  }

  m_fxImpl->UpdateBuffers(currentBuff, nextBuff);
}

FlyingStarsFx::FlyingStarsImpl::FlyingStarsImpl(std::shared_ptr<const PluginInfo> info) noexcept
  : m_goomInfo{std::move(info)},
    m_halfWidth{static_cast<int32_t>(m_goomInfo->GetScreenInfo().width / 2)},
    m_halfHeight{static_cast<int32_t>(m_goomInfo->GetScreenInfo().height / 2)},
    m_draw{m_goomInfo->GetScreenInfo().width, m_goomInfo->GetScreenInfo().height},
    m_drawFuncs{{DrawMode::CIRCLES,
                 [&](const std::vector<PixelBuffer*>& buffs,
                     const int32_t x1,
                     const int32_t y1,
                     const int32_t x2,
                     const int32_t y2,
                     const uint32_t size,
                     const std::vector<Pixel>& colors) {
                   DrawParticleCircle(buffs, x1, y1, x2, y2, size, colors);
                 }},
                {DrawMode::LINES,
                 [&](const std::vector<PixelBuffer*>& buffs,
                     const int32_t x1,
                     const int32_t y1,
                     const int32_t x2,
                     const int32_t y2,
                     const uint32_t size,
                     const std::vector<Pixel>& colors) {
                   DrawParticleLine(buffs, x1, y1, x2, y2, size, colors);
                 }},
                {
                    DrawMode::DOTS,
                    [&](const std::vector<PixelBuffer*>& buffs,
                        const int32_t x1,
                        const int32_t y1,
                        const int32_t x2,
                        const int32_t y2,
                        const uint32_t size,
                        const std::vector<Pixel>& colors) {
                      DrawParticleDot(buffs, x1, y1, x2, y2, size, colors);
                    },
                }}
{
  m_stars.reserve(MAX_NUM_STARS);

  m_dominantColorMapID = m_randomColorMapsManager.AddColorMapInfo(
      {m_colorMaps, ColorMapName::_NULL, RandomColorMaps::ALL});
  m_dominantLowColorMapID = m_randomColorMapsManager.AddColorMapInfo(
      {m_lowColorMaps, ColorMapName::_NULL, RandomColorMaps::ALL});
  m_colorMapID = m_randomColorMapsManager.AddColorMapInfo(
      {m_colorMaps, ColorMapName::_NULL, RandomColorMaps::ALL});
  m_lowColorMapID = m_randomColorMapsManager.AddColorMapInfo(
      {m_lowColorMaps, ColorMapName::_NULL, RandomColorMaps::ALL});

  constexpr float MIN_SATURATION = 0.5F;
  constexpr float MAX_SATURATION = 1.0F;
  m_colorMaps->SetSaturationLimts(MIN_SATURATION, MAX_SATURATION);
  m_lowColorMaps->SetSaturationLimts(MIN_SATURATION, MAX_SATURATION);

  constexpr float MIN_LIGHTNESS = 0.5F;
  constexpr float MAX_LIGHTNESS = 1.0F;
  m_colorMaps->SetLightnessLimits(MIN_LIGHTNESS, MAX_LIGHTNESS);
  m_lowColorMaps->SetLightnessLimits(MIN_LIGHTNESS, MAX_LIGHTNESS);
}

inline auto FlyingStarsFx::FlyingStarsImpl::GetResourcesDirectory() const -> const std::string&
{
  return m_resourcesDirectory;
}

inline void FlyingStarsFx::FlyingStarsImpl::SetResourcesDirectory(const std::string& dirName)
{
  m_resourcesDirectory = dirName;
}

void FlyingStarsFx::FlyingStarsImpl::SetBuffSettings(const FXBuffSettings& settings)
{
  m_draw.SetBuffIntensity(settings.buffIntensity);
  m_draw.SetAllowOverexposed(settings.allowOverexposed);
}

inline void FlyingStarsFx::FlyingStarsImpl::SetSmallImageBitmaps(
    const SmallImageBitmaps& smallBitmaps)
{
  m_smallBitmaps = &smallBitmaps;
}

inline auto FlyingStarsFx::FlyingStarsImpl::GetImageBitmap(const size_t size) -> const ImageBitmap&
{
  return m_smallBitmaps->GetImageBitmap(
      SmallImageBitmaps::ImageNames::CIRCLE,
      stdnew::clamp(size % 2 != 0 ? size : size + 1, MIN_DOT_SIZE, MAX_DOT_SIZE));
}

inline void FlyingStarsFx::FlyingStarsImpl::Start()
{
  if (m_smallBitmaps == nullptr)
  {
    throw std::logic_error("FlyingStarsFx: Starting without setting 'm_smallBitmaps'.");
  }
}

void FlyingStarsFx::FlyingStarsImpl::Finish()
{
  m_stats.SetLastNumActive(m_stars.size());
  m_stats.SetLastMaxStars(m_maxStars);
  m_stats.SetLastMaxStarAge(m_maxStarAge);
}

void FlyingStarsFx::FlyingStarsImpl::Log(const GoomStats::LogStatsValueFunc& logVal) const
{
  m_stats.Log(logVal);
}

void FlyingStarsFx::FlyingStarsImpl::UpdateBuffers(PixelBuffer& currentBuff, PixelBuffer& nextBuff)
{
  m_counter++;

  m_maxStars = GetRandInRange(MIN_NUM_STARS, MAX_NUM_STARS);

  CheckForStarEvents();
  DrawStars(currentBuff, nextBuff);
  RemoveDeadStars();
}

void FlyingStarsFx::FlyingStarsImpl::CheckForStarEvents()
{
  if (m_stars.empty() || m_goomInfo->GetSoundInfo().GetTimeSinceLastGoom() < 1)
  {
    SoundEventOccurred();
    if (ProbabilityOfMInN(1, 20))
    {
      // Give a slight weight towards noFx mode by using numFX + 2.
      const uint32_t newVal = GetNRand(NUM_FX + 2);
      m_fxMode = newVal >= NUM_FX ? StarModes::NO_FX : static_cast<StarModes>(newVal);
      ChangeColorMode();
      ChangeDrawMode();
    }
    else if (m_counter > MAX_COUNT)
    {
      m_counter = 0;
      ChangeColorMode();
      ChangeDrawMode();
    }
  }
  // m_fxMode = StarModes::rain;
}

void FlyingStarsFx::FlyingStarsImpl::DrawStars(PixelBuffer& currentBuff, PixelBuffer& nextBuff)
{
  m_stats.UpdateStars();
  const float flipSpeed = GetRandInRange(0.1F, 10.0F);

  for (auto& star : m_stars)
  {
    UpdateStar(star);

    // Is it a dead particle?
    if (star.age >= static_cast<float>(m_maxStarAge))
    {
      m_stats.DeadStar();
      continue;
    }

    DrawParticle(currentBuff, nextBuff, star, flipSpeed, GetDrawFunc());
  }
}

void FlyingStarsFx::FlyingStarsImpl::ChangeDrawMode()
{
  // clang-format off
  static const Weights<DrawMode> s_drawModeWeights{{
      { DrawMode::DOTS,              30 },
      { DrawMode::CIRCLES,           20 },
      { DrawMode::LINES,             10 },
      { DrawMode::CIRCLES_AND_LINES, 15 },
  }};
  // clang-format on

  m_drawMode = s_drawModeWeights.GetRandomWeighted();
}


inline auto FlyingStarsFx::FlyingStarsImpl::GetDrawFunc() const -> DrawFunc
{
  if (m_drawMode != DrawMode::CIRCLES_AND_LINES)
  {
    return m_drawFuncs.at(m_drawMode);
  }
  return ProbabilityOfMInN(1, 2) ? m_drawFuncs.at(DrawMode::CIRCLES)
                                 : m_drawFuncs.at(DrawMode::LINES);
}

void FlyingStarsFx::FlyingStarsImpl::DrawParticle(PixelBuffer& currentBuff,
                                                  PixelBuffer& nextBuff,
                                                  const Star& star,
                                                  const float flipSpeed,
                                                  const DrawFunc& drawFunc)
{
  const float tAge = star.age / static_cast<float>(m_maxStarAge);
  const float ageBrightness = 0.2F + 0.8F * Sq(0.5F - tAge) / 0.25F;
  const size_t numParts =
      tAge > OLD_AGE ? 4 : 2 + static_cast<size_t>(std::lround((1.0F - tAge) * 2.0F));

  const auto x0 = static_cast<int32_t>(star.pos.x);
  const auto y0 = static_cast<int32_t>(star.pos.y);

  int32_t x1 = x0;
  int32_t y1 = y0;
  for (size_t j = 1; j <= numParts; j++)
  {
    const int32_t x2 =
        x0 - static_cast<int32_t>(
                 0.5 * (1.0 + std::sin(flipSpeed * star.velocity.x * static_cast<float>(j))) *
                 star.velocity.x * static_cast<float>(j));
    const int32_t y2 =
        y0 - static_cast<int32_t>(
                 0.5 * (1.0 + std::cos(flipSpeed * star.velocity.y * static_cast<float>(j))) *
                 star.velocity.y * static_cast<float>(j));

    const float brightness = ageBrightness * static_cast<float>(j) / static_cast<float>(numParts);
#if __cplusplus <= 201402L
    const auto mixedColors = GetMixedColors(star, tAge, brightness);
    const auto mixedColor = std::get<0>(mixedColors);
    const auto mixedLowColor = std::get<1>(mixedColors);
#else
    const auto [mixedColor, mixedLowColor] = GetMixedColors(star, tAge, brightness);
#endif
    const std::vector<Pixel> colors = {mixedColor, mixedLowColor};
    const std::vector<PixelBuffer*> buffs{&currentBuff, &nextBuff};
    const uint32_t size = tAge < OLD_AGE ? 1 : GetRandInRange(2U, MAX_DOT_SIZE + 1);

    drawFunc(buffs, x1, y1, x2, y2, size, colors);

    x1 = x2;
    y1 = y2;
  }
}

void FlyingStarsFx::FlyingStarsImpl::DrawParticleCircle(const std::vector<PixelBuffer*>& buffs,
                                                        const int32_t x1,
                                                        const int32_t y1,
                                                        [[maybe_unused]] const int32_t x2,
                                                        [[maybe_unused]] const int32_t y2,
                                                        const uint32_t size,
                                                        const std::vector<Pixel>& colors)
{
  if (m_useSingleBufferOnly)
  {
    m_draw.Circle(*(buffs[0]), x1, y1, static_cast<int>(size), colors[0]);
  }
  else
  {
    m_draw.Circle(buffs, x1, y1, static_cast<int>(size), colors);
  }
}

void FlyingStarsFx::FlyingStarsImpl::DrawParticleLine(const std::vector<PixelBuffer*>& buffs,
                                                      const int32_t x1,
                                                      const int32_t y1,
                                                      const int32_t x2,
                                                      const int32_t y2,
                                                      const uint32_t size,
                                                      const std::vector<Pixel>& colors)
{
  if (m_useSingleBufferOnly)
  {
    m_draw.Line(*(buffs[0]), x1, y1, x2, y2, colors[0], static_cast<int>(size));
  }
  else
  {
    m_draw.Line(buffs, x1, y1, x2, y2, colors, static_cast<int>(size));
  }
}

void FlyingStarsFx::FlyingStarsImpl::DrawParticleDot(const std::vector<PixelBuffer*>& buffs,
                                                     const int32_t x1,
                                                     const int32_t y1,
                                                     [[maybe_unused]] const int32_t x2,
                                                     [[maybe_unused]] const int32_t y2,
                                                     const uint32_t size,
                                                     const std::vector<Pixel>& colors)
{
  const auto getColor = [&]([[maybe_unused]] const size_t x, [[maybe_unused]] const size_t y,
                            const Pixel& b) -> Pixel {
    return GetColorMultiply(b, colors[0], m_buffSettings.allowOverexposed);
  };
  const auto getLowColor = [&]([[maybe_unused]] const size_t x, [[maybe_unused]] const size_t y,
                               const Pixel& b) -> Pixel {
    return GetColorMultiply(b, colors[1], m_buffSettings.allowOverexposed);
  };

  const ImageBitmap& bitmap1 = GetImageBitmap(size);

  if (m_useSingleBufferOnly)
  {
    m_draw.Bitmap(*(buffs[0]), x1, y1, bitmap1, getColor);
  }
  else
  {
    const std::vector<GoomDraw::GetBitmapColorFunc> getColors{getColor, getLowColor};
    const std::vector<const PixelBuffer*> bitmaps{&bitmap1, &bitmap1};
    m_draw.Bitmap(buffs, x1, y1, bitmaps, getColors);
  }
}

void FlyingStarsFx::FlyingStarsImpl::RemoveDeadStars()
{
  const auto isDead = [&](const Star& s) { return IsStarDead(s); };
#if __cplusplus <= 201402L
  m_stars.erase(std::remove_if(m_stars.begin(), m_stars.end(), isDead), m_stars.end());
#else
  const size_t numRemoved = std::erase_if(m_stars, isDead);
  if (numRemoved > 0)
  {
    m_stats.RemovedDeadStars(numRemoved);
  }
#endif
}

inline auto FlyingStarsFx::FlyingStarsImpl::IsStarDead(const Star& s) const -> bool
{
  constexpr int32_t DEAD_MARGIN = 64;

  return (s.pos.x < -DEAD_MARGIN) ||
         (s.pos.x > static_cast<float>(m_goomInfo->GetScreenInfo().width + DEAD_MARGIN)) ||
         (s.pos.y < -DEAD_MARGIN) ||
         (s.pos.y > static_cast<float>(m_goomInfo->GetScreenInfo().height + DEAD_MARGIN)) ||
         (s.age >= static_cast<float>(this->m_maxStarAge));
}

void FlyingStarsFx::FlyingStarsImpl::ChangeColorMaps()
{
  m_randomColorMapsManager.ChangeColorMapNow(m_dominantColorMapID);
  m_randomColorMapsManager.ChangeColorMapNow(m_dominantLowColorMapID);

  const bool megaColorMode = ProbabilityOfMInN(1, 10);
  if (megaColorMode)
  {
    m_randomColorMapsManager.UpdateColorMapName(m_colorMapID, ColorMapName::_NULL);
    m_randomColorMapsManager.UpdateColorMapName(m_lowColorMapID, ColorMapName::_NULL);
  }
  else
  {
    m_randomColorMapsManager.UpdateColorMapName(m_colorMapID, m_colorMaps->GetRandomColorMapName());
    m_randomColorMapsManager.UpdateColorMapName(m_lowColorMapID,
                                                m_lowColorMaps->GetRandomColorMapName());
  }

  UpdateAngleColorMapNames();
}

void FlyingStarsFx::FlyingStarsImpl::UpdateStarColorMaps(const float angle, Star& star)
{
  // TODO Get colormap based on current mode.
  if (ProbabilityOfMInN(10, 20))
  {
    star.dominantColormap = m_randomColorMapsManager.GetColorMapPtr(m_dominantColorMapID);
    star.dominantLowColormap = m_randomColorMapsManager.GetColorMapPtr(m_dominantLowColorMapID);
    star.currentColorMap = m_randomColorMapsManager.GetColorMapPtr(m_colorMapID);
    star.currentLowColorMap = m_randomColorMapsManager.GetColorMapPtr(m_lowColorMapID);
  }
  else
  {
    star.dominantColormap = GetDominantColorMapPtr(angle);
    star.dominantLowColormap = GetDominantLowColorMapPtr(angle);
    star.currentColorMap = GetCurrentColorMapPtr(angle);
    star.currentLowColorMap = GetCurrentLowColorMapPtr(angle);
  }
}

void FlyingStarsFx::FlyingStarsImpl::UpdateAngleColorMapNames()
{
  for (size_t i = 0; i < NUM_SEGMENTS; ++i)
  {
    m_angleColorMapName[i] = m_colorMaps->GetRandomColorMapName();
  }
}

auto FlyingStarsFx::FlyingStarsImpl::GetDominantColorMapPtr(const float angle) const
    -> std::shared_ptr<const IColorMap>
{
  return std::const_pointer_cast<const IColorMap>(
      m_colorMaps->GetColorMapPtr(m_angleColorMapName[GetSegmentNum(angle)]));
}

auto FlyingStarsFx::FlyingStarsImpl::GetDominantLowColorMapPtr(const float angle) const
    -> std::shared_ptr<const IColorMap>
{
  return std::const_pointer_cast<const IColorMap>(
      m_colorMaps->GetColorMapPtr(m_angleColorMapName[GetSegmentNum(angle)]));
}

auto FlyingStarsFx::FlyingStarsImpl::GetCurrentColorMapPtr(const float angle) const
    -> std::shared_ptr<const IColorMap>
{
  return std::const_pointer_cast<const IColorMap>(
      m_colorMaps->GetColorMapPtr(m_angleColorMapName[GetSegmentNum(angle)]));
}

auto FlyingStarsFx::FlyingStarsImpl::GetCurrentLowColorMapPtr(const float angle) const
    -> std::shared_ptr<const IColorMap>
{
  return std::const_pointer_cast<const IColorMap>(
      m_colorMaps->GetColorMapPtr(m_angleColorMapName[GetSegmentNum(angle)]));
}

auto FlyingStarsFx::FlyingStarsImpl::GetSegmentNum(const float angle) -> size_t
{
  const float segmentSize = m_two_pi / static_cast<float>(NUM_SEGMENTS);
  float a = segmentSize;
  for (size_t i = 0; i < NUM_SEGMENTS; ++i)
  {
    if (angle <= a)
    {
      return i;
    }
    a += segmentSize;
  }
  throw std::logic_error("Angle too large.");
}

void FlyingStarsFx::FlyingStarsImpl::ChangeColorMode()
{
  // clang-format off
  static const Weights<ColorMode> s_colorModeWeights{{
    { ColorMode::MIX_COLORS,         30 },
    { ColorMode::REVERSE_MIX_COLORS, 15 },
    { ColorMode::SIMILAR_LOW_COLORS, 10 },
    { ColorMode::SINE_MIX_COLORS,     5 },
  }};
  // clang-format on

  m_colorMode = s_colorModeWeights.GetRandomWeighted();
}

inline auto FlyingStarsFx::FlyingStarsImpl::GetMixedColors(const Star& star,
                                                           const float t,
                                                           const float brightness)
    -> std::tuple<Pixel, Pixel>
{
  constexpr float STAR_GAMMA = 4.2;
  constexpr float STAR_GAMMA_THRESHOLD = 0.1;
  static GammaCorrection s_gammaCorrect{STAR_GAMMA, STAR_GAMMA_THRESHOLD};

  Pixel color;
  Pixel lowColor;
  Pixel dominantColor;
  Pixel dominantLowColor;

  switch (m_colorMode)
  {
    case ColorMode::SINE_MIX_COLORS:
    {
      constexpr float INITIAL_FREQ = 20.0;
      constexpr float T_MIX_FACTOR = 0.5;
      constexpr float Z_STEP = 0.1;
      static float s_freq = INITIAL_FREQ;
      static const float s_zStep = Z_STEP;
      static float s_z = 0.0;

      const float tSin = T_MIX_FACTOR * (1.0F + std::sin(s_freq * s_z));
      color = star.currentColorMap->GetColor(tSin);
      lowColor = star.currentLowColorMap->GetColor(tSin);
      dominantColor = star.dominantColormap->GetColor(tSin);
      dominantLowColor = star.dominantLowColormap->GetColor(tSin);

      s_z += s_zStep;
      break;
    }
    case ColorMode::MIX_COLORS:
    case ColorMode::SIMILAR_LOW_COLORS:
    {
      color = star.currentColorMap->GetColor(t);
      lowColor = star.currentLowColorMap->GetColor(t);
      dominantColor = star.dominantColormap->GetColor(t);
      if (m_colorMode == ColorMode::SIMILAR_LOW_COLORS)
      {
        dominantLowColor = dominantColor;
      }
      else
      {
        dominantLowColor = star.dominantLowColormap->GetColor(t);
      }
      break;
    }
    case ColorMode::REVERSE_MIX_COLORS:
    {
      color = star.currentColorMap->GetColor(1.0F - t);
      lowColor = star.currentLowColorMap->GetColor(1.0F - t);
      dominantColor = star.dominantColormap->GetColor(1.0F - t);
      dominantLowColor = star.dominantLowColormap->GetColor(1.0F - t);
      break;
    }
    default:
      throw std::logic_error("Unknown ColorMode enum.");
  }

  constexpr float MIN_MIX = 0.2;
  constexpr float MAX_MIX = 0.8;
  const float tMix = stdnew::lerp(MIN_MIX, MAX_MIX, t);
  const Pixel mixedColor =
      s_gammaCorrect.GetCorrection(brightness, IColorMap::GetColorMix(color, dominantColor, tMix));
  const Pixel mixedLowColor =
      GetLightenedColor(IColorMap::GetColorMix(lowColor, dominantLowColor, tMix), 10.0F);
  const Pixel remixedLowColor =
      m_colorMode == ColorMode::SIMILAR_LOW_COLORS
          ? mixedLowColor
          : s_gammaCorrect.GetCorrection(brightness,
                                         IColorMap::GetColorMix(mixedColor, mixedLowColor, 0.4));

  return std::make_tuple(mixedColor, remixedLowColor);
}

/**
 * Met a jour la position et vitesse d'une particule.
 */
void FlyingStarsFx::FlyingStarsImpl::UpdateStar(Star& s)
{
  s.pos += s.velocity;
  s.velocity += s.acceleration;
  s.age += s.vage;
}

/**
 * Ajoute de nouvelles particules au moment d'un evenement sonore.
 */
void FlyingStarsFx::FlyingStarsImpl::SoundEventOccurred()
{
  m_stats.SoundEventOccurred();

  if (m_fxMode == StarModes::NO_FX)
  {
    m_stats.NoFxChosen();
    return;
  }

  m_maxStarAge = MIN_STAR_AGE + GetNRand(MAX_STAR_EXTRA_AGE);
  m_useSingleBufferOnly = ProbabilityOfMInN(1, 100);

  UpdateWindAndGravity();
  ChangeColorMaps();

  // Why 200 ? Because the FX was developed on 320x200.
  constexpr float WIDTH = 320.0;
  constexpr float HEIGHT = 200.0;
  constexpr float MIN_HEIGHT = 50.0;
  const auto heightRatio = static_cast<float>(m_goomInfo->GetScreenInfo().height) / HEIGHT;
  const float defaultRadius = (1.0F + m_goomInfo->GetSoundInfo().GetGoomPower()) *
                              GetRandInRange(MIN_HEIGHT, HEIGHT) / WIDTH;

  const StarModeParams starParams = GetStarParams(defaultRadius, heightRatio);
  const size_t maxStarsInBomb = GetMaxStarsInABomb(heightRatio);

  AddStarBombs(starParams, maxStarsInBomb);
}

void FlyingStarsFx::FlyingStarsImpl::UpdateWindAndGravity()
{
  if (ProbabilityOfMInN(1, 10))
  {
    m_minSideWind = GetRandInRange(MIN_MIN_SIDE_WIND, MAX_MIN_SIDE_WIND);
    m_maxSideWind = GetRandInRange(MIN_MAX_SIDE_WIND, MAX_MAX_SIDE_WIND);
    m_minGravity = GetRandInRange(MIN_MIN_GRAVITY, MAX_MIN_GRAVITY);
    m_maxGravity = GetRandInRange(MIN_MAX_GRAVITY, MAX_MAX_GRAVITY);
  }
}

auto FlyingStarsFx::FlyingStarsImpl::GetMaxStarsInABomb(const float heightRatio) const -> size_t
{
  auto maxStarsInBomb = static_cast<size_t>(
      heightRatio *
      (100.0F + (1.0F + m_goomInfo->GetSoundInfo().GetGoomPower()) * GetRandInRange(0.0F, 150.0F)));

  if (m_goomInfo->GetSoundInfo().GetTimeSinceLastBigGoom() < 1)
  {
    return 2 * maxStarsInBomb;
  }

  return maxStarsInBomb;
}

auto FlyingStarsFx::FlyingStarsImpl::GetStarParams(const float defaultRadius,
                                                   const float heightRatio) -> StarModeParams
{
  StarModeParams starParams;

  switch (m_fxMode)
  {
    case StarModes::FIREWORKS:
      m_stats.FireworksFxChosen();
      starParams = GetFireworksStarParams(defaultRadius);
      break;
    case StarModes::RAIN:
      m_stats.RainFxChosen();
      starParams = GetRainStarParams(defaultRadius);
      break;
    case StarModes::FOUNTAIN:
      m_stats.FountainFxChosen();
      m_maxStarAge *= 2.0 / 3.0;
      starParams = GetFountainStarParams(defaultRadius);
      break;
    default:
      throw std::logic_error("Unknown StarModes enum.");
  }

  starParams.radius *= heightRatio;
  if (m_goomInfo->GetSoundInfo().GetTimeSinceLastBigGoom() < 1)
  {
    starParams.radius *= 1.5;
  }

  return starParams;
}

auto FlyingStarsFx::FlyingStarsImpl::GetFireworksStarParams(const float defaultRadius) const
    -> StarModeParams
{
  StarModeParams starParams;

  const auto rsq = static_cast<double>(m_halfHeight * m_halfHeight);
  while (true)
  {
    starParams.pos.x = static_cast<int32_t>(GetNRand(m_goomInfo->GetScreenInfo().width));
    starParams.pos.y = static_cast<int32_t>(GetNRand(m_goomInfo->GetScreenInfo().height));
    const double sqDist = SqDistance(static_cast<float>(starParams.pos.x - m_halfWidth),
                                     static_cast<float>(starParams.pos.y - m_halfHeight));
    if (sqDist < rsq)
    {
      break;
    }
  }

  constexpr float RADIUS_FACTOR = 1.0;
  constexpr float INITIAL_WIND_FACTOR = 0.1;
  constexpr float INITIAL_GRAVITY_FACTOR = 0.4;
  starParams.radius = RADIUS_FACTOR * defaultRadius;
  starParams.vage = m_maxAge * (1.0F - m_goomInfo->GetSoundInfo().GetGoomPower());
  starParams.windFactor = INITIAL_WIND_FACTOR;
  starParams.gravityFactor = INITIAL_GRAVITY_FACTOR;

  return starParams;
}

auto FlyingStarsFx::FlyingStarsImpl::GetRainStarParams(const float defaultRadius) const
    -> StarModeParams
{
  StarModeParams starParams;

  const auto x0 = static_cast<int32_t>(m_goomInfo->GetScreenInfo().width / 25);
  starParams.pos.x = static_cast<int32_t>(
      GetRandInRange(x0, static_cast<int32_t>(m_goomInfo->GetScreenInfo().width) - x0));
  starParams.pos.y = -GetRandInRange(3, 64);

  constexpr float RADIUS_FACTOR = 1.5;
  constexpr float INITIAL_VAGE = 0.002;
  constexpr float INITIAL_WIND_FACTOR = 1.0;
  constexpr float INITIAL_GRAVITY_FACTOR = 0.4;
  starParams.radius = RADIUS_FACTOR * defaultRadius;
  starParams.vage = INITIAL_VAGE;
  starParams.windFactor = INITIAL_WIND_FACTOR;
  starParams.gravityFactor = INITIAL_GRAVITY_FACTOR;

  return starParams;
}

auto FlyingStarsFx::FlyingStarsImpl::GetFountainStarParams(const float defaultRadius) const
    -> StarModeParams
{
  StarModeParams starParams;

  const int32_t x0 = m_halfWidth / 5;
  starParams.pos.x = GetRandInRange(m_halfWidth - x0, m_halfWidth + x0);
  starParams.pos.y =
      static_cast<int32_t>(m_goomInfo->GetScreenInfo().height + GetRandInRange(3U, 64U));

  constexpr float INITIAL_VAGE = 0.001;
  constexpr float INITIAL_WIND_FACTOR = 1.0;
  constexpr float INITIAL_GRAVITY_FACTOR = 1.0;
  starParams.radius = 1.0F + defaultRadius;
  starParams.vage = INITIAL_VAGE;
  starParams.windFactor = INITIAL_WIND_FACTOR;
  starParams.gravityFactor = INITIAL_GRAVITY_FACTOR;

  return starParams;
}

void FlyingStarsFx::FlyingStarsImpl::AddStarBombs(const StarModeParams& starModeParams,
                                                  const size_t maxStarsInBomb)
{
  const float sideWind = starModeParams.windFactor * GetRandInRange(m_minSideWind, m_maxSideWind);
  const float gravity = starModeParams.gravityFactor * GetRandInRange(m_minGravity, m_maxGravity);

  for (size_t i = 0; i < maxStarsInBomb; i++)
  {
    m_randomColorMapsManager.ChangeColorMapNow(m_colorMapID);
    m_randomColorMapsManager.ChangeColorMapNow(m_lowColorMapID);
    AddABomb(starModeParams.pos, starModeParams.radius, starModeParams.vage, gravity, sideWind);
  }
}

/**
 * Cree une nouvelle 'bombe', c'est a dire une particule appartenant a une fusee d'artifice.
 */
void FlyingStarsFx::FlyingStarsImpl::AddABomb(const V2dInt& pos,
                                              const float radius,
                                              const float vage,
                                              const float gravity,
                                              const float sideWind)
{
  if (m_stars.size() >= m_maxStars)
  {
    m_stats.AddBombButTooManyStars();
    return;
  }
  m_stats.AddBomb();

  m_stars.emplace_back(Star{});
  const size_t i = m_stars.size() - 1;

  m_stars[i].pos = pos.ToFlt();

  const float bombRadius = radius * GetRandInRange(0.01F, 2.0F);
  const float bombAngle = GetBombAngle(m_stars[i]);

  constexpr float RADIUS_OFFSET = -0.2;
  m_stars[i].velocity.x = bombRadius * std::cos(bombAngle);
  m_stars[i].velocity.y = RADIUS_OFFSET + bombRadius * std::sin(bombAngle);

  m_stars[i].acceleration.x = sideWind;
  m_stars[i].acceleration.y = gravity;

  m_stars[i].age = GetRandInRange(m_minAge, 0.5F * m_maxAge);
  m_stars[i].vage = std::max(m_minAge, vage);

  UpdateStarColorMaps(bombAngle, m_stars[i]);
}

auto FlyingStarsFx::FlyingStarsImpl::GetBombAngle(const Star& star) const -> float
{
  float minAngle;
  float maxAngle;

  switch (m_fxMode)
  {
    case StarModes::FIREWORKS:
      minAngle = 0.0;
      maxAngle = m_two_pi;
      break;
    case StarModes::RAIN:
    {
      constexpr float MIN_RAIN_ANGLE = 0.1;
      constexpr float MAX_RAIN_ANGLE = m_pi - 0.1;
      const float xFactor = star.pos.x / static_cast<float>(m_goomInfo->GetScreenInfo().width - 1);
      minAngle = stdnew::lerp(MIN_RAIN_ANGLE, m_half_pi - 0.1F, 1.0F - xFactor);
      maxAngle = stdnew::lerp(m_half_pi + 0.1F, MAX_RAIN_ANGLE, xFactor);
      break;
    }
    case StarModes::FOUNTAIN:
      minAngle = 1.0F * m_pi / 3.0F;
      maxAngle = 5.0F * m_pi / 3.0F;
      break;
    default:
      throw std::logic_error("Unknown StarModes enum.");
  }

  return GetRandInRange(minAngle, maxAngle);
}

} // namespace GOOM
