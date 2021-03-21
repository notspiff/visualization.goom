#include "image_fx.h"

#include "goom_draw.h"
#include "goom_graphic.h"
#include "goom_plugin_info.h"
#include "goom_stats.h"
#include "goom_visual_fx.h"
#include "goomutils/colormaps.h"
#include "goomutils/colorutils.h"
#include "goomutils/image_bitmaps.h"
#include "goomutils/logging_control.h"
#undef NO_LOGGING
#include "goomutils/goomrand.h"
#include "goomutils/logging.h"
#include "goomutils/mathutils.h"
#include "goomutils/random_colormaps.h"
#include "v2d.h"

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace GOOM
{

using namespace GOOM::UTILS;

struct Image
{
  Image(std::shared_ptr<ImageBitmap> i,
        std::shared_ptr<const IColorMap> cm,
        const V2dInt& c,
        const int p,
        const int step)
    : image{std::move(i)},
      colorMap{std::move(cm)},
      startingCentre{c},
      pathCounter{p},
      counterStep{step}
  {
  }

  const std::shared_ptr<ImageBitmap> image;
  std::shared_ptr<const IColorMap> colorMap{};
  const V2dInt startingCentre;
  V2dInt centre{};
  int pathCounter{};
  int counterStep{};
  bool backwards{false};
};

class ImageFx::ImageFxImpl
{
public:
  explicit ImageFxImpl(const std::shared_ptr<const PluginInfo>& info) noexcept;
  ~ImageFxImpl() noexcept = default;
  ImageFxImpl(const ImageFxImpl&) noexcept = delete;
  ImageFxImpl(ImageFxImpl&&) noexcept = delete;
  auto operator=(const ImageFxImpl&) -> ImageFxImpl& = delete;
  auto operator=(ImageFxImpl&&) -> ImageFxImpl& = delete;

  [[nodiscard]] auto GetResourcesDirectory() const -> const std::string&;
  void SetResourcesDirectory(const std::string& dirName);

  void SetBuffSettings(const FXBuffSettings& settings);

  void Start();

  // void Apply(PixelBuffer& currentBuff);
  void Apply(PixelBuffer& currentBuff, PixelBuffer& nextBuff);

private:
  std::shared_ptr<const PluginInfo> m_goomInfo{};
  std::string m_resourcesDirectory{};
  FXBuffSettings m_buffSettings{};
  uint64_t m_updateNum{};
  GoomDraw m_draw;
  RandomColorMaps m_colorMaps{};
  std::vector<Image> m_images{};
  const V2dInt m_targetPos;
  auto GetNextStartingCentre(Image& image) -> V2dInt;
  static constexpr uint32_t MAX_PATH_COUNT = 500;
  std::vector<V2dInt> m_path;
  void InitPath();
  float m_tVal{0};
  bool m_tValBack = false;
  static constexpr float T_STEP = 0.01;
  void ResetCentres();
  void UpdateCentres();
  GammaCorrection m_gammaCorrect{2.0, 0.01};
};

ImageFx::ImageFx(const std::shared_ptr<const PluginInfo>& info) noexcept
  : m_fxImpl{new ImageFxImpl{info}}
{
}

ImageFx::~ImageFx() noexcept = default;

auto ImageFx::GetResourcesDirectory() const -> const std::string&
{
  return m_fxImpl->GetResourcesDirectory();
}

void ImageFx::SetResourcesDirectory(const std::string& dirName)
{
  m_fxImpl->SetResourcesDirectory(dirName);
}

void ImageFx::SetBuffSettings(const FXBuffSettings& settings)
{
  m_fxImpl->SetBuffSettings(settings);
}

void ImageFx::Start()
{
  m_fxImpl->Start();
}

void ImageFx::Log([[maybe_unused]] const GoomStats::LogStatsValueFunc& l) const
{
}

void ImageFx::Finish()
{
}

auto ImageFx::GetFxName() const -> std::string
{
  return "image";
}

/**
void ImageFx::Apply(PixelBuffer& currentBuff)
{
  if (!m_enabled)
  {
    return;
  }
  m_fxImpl->Apply(currentBuff);
}
 **/

void ImageFx::Apply(PixelBuffer& currentBuff, PixelBuffer& nextBuffer)
{
  if (!m_enabled)
  {
    return;
  }
  m_fxImpl->Apply(currentBuff, nextBuffer);
}

ImageFx::ImageFxImpl::ImageFxImpl(const std::shared_ptr<const PluginInfo>& info) noexcept
  : m_goomInfo(info),
    m_draw{m_goomInfo->GetScreenInfo().width, m_goomInfo->GetScreenInfo().height},
    m_targetPos{static_cast<int>(m_goomInfo->GetScreenInfo().width / 2),
                static_cast<int>(m_goomInfo->GetScreenInfo().height / 2)},
    m_path(MAX_PATH_COUNT + 1)
{
  InitPath();
}

void ImageFx::ImageFxImpl::Start()
{
  m_updateNum = 0;

  auto image = std::make_shared<ImageBitmap>(m_resourcesDirectory + PATH_SEP + IMAGES_DIR +
                                             PATH_SEP + "mountain_sunset100x65.png");
  m_images.emplace_back(
      Image{image, m_colorMaps.GetRandomColorMapPtr(), {200, 200}, MAX_PATH_COUNT / 4, 2});
  m_images.emplace_back(Image{image,
                              m_colorMaps.GetRandomColorMapPtr(),
                              {200, static_cast<int>(m_goomInfo->GetScreenInfo().height) - 200},
                              2 * MAX_PATH_COUNT / 4,
                              1});
  m_images.emplace_back(Image{image,
                              m_colorMaps.GetRandomColorMapPtr(),
                              {static_cast<int>(m_goomInfo->GetScreenInfo().width) - 200,
                               static_cast<int>(m_goomInfo->GetScreenInfo().height) - 200},
                              3 * MAX_PATH_COUNT / 4,
                              2});
  m_images.emplace_back(Image{image,
                              m_colorMaps.GetRandomColorMapPtr(),
                              {static_cast<int>(m_goomInfo->GetScreenInfo().width) - 200, 200},
                              4 * MAX_PATH_COUNT / 4,
                              3});
}

auto ImageFx::ImageFxImpl::GetResourcesDirectory() const -> const std::string&
{
  return m_resourcesDirectory;
}

void ImageFx::ImageFxImpl::SetResourcesDirectory(const std::string& dirName)
{
  m_resourcesDirectory = dirName;
}

void ImageFx::ImageFxImpl::SetBuffSettings(const FXBuffSettings& settings)
{
  m_buffSettings = settings;
  m_draw.SetBuffIntensity(m_buffSettings.buffIntensity);
  m_draw.SetAllowOverexposed(m_buffSettings.allowOverexposed);
}

void ImageFx::ImageFxImpl::Apply(PixelBuffer& currentBuff, PixelBuffer& nextBuff)
{
  m_updateNum++;

  if (m_updateNum % 100 == 0)
  {
    for (auto& image : m_images)
    {
      image.colorMap = m_colorMaps.GetRandomColorMapPtr();
    }
  }

  constexpr float BRIGHTNESS = 0.20F;
  size_t imageIndex = 0;
  const auto getColor = [&]([[maybe_unused]] const int x, [[maybe_unused]] const int y,
                            const Pixel& b) -> Pixel {
    return GetBrighterColor(
        BRIGHTNESS, GetColorMultiply(b, m_images[imageIndex].colorMap->GetColor(m_tVal), false),
        //        GetColorBlend(b, m_colorMap->GetColor(m_tVal)),
        false);
  };
  const auto getLighterColor = [&]([[maybe_unused]] const int x, [[maybe_unused]] const int y,
                                   const Pixel& b) -> Pixel {
    return GetLightenedColor(getColor(x, y, b), 10);
  };


  const std::vector<PixelBuffer*> buffs{&currentBuff, &nextBuff};
  const std::vector<GoomDraw::GetBitmapColorFunc> getColors{getColor, getLighterColor};
  for (size_t i = 0; i < m_images.size(); ++i)
  {
    const std::vector<const PixelBuffer*> bitmaps{m_images[i].image.get(), m_images[i].image.get()};
    imageIndex = i;
    m_draw.Bitmap(buffs, m_images[i].centre.x, m_images[i].centre.y, bitmaps, getColors, 2, 2);
  }

  m_tVal += m_tValBack ? -T_STEP : +T_STEP;
  if (m_tVal > 1.0)
  {
    m_tValBack = true;
    //    ResetCentres();
  }
  else if (m_tVal < 0.0)
  {
    m_tValBack = false;
    //    ResetCentres();
  }
  UpdateCentres();
}

void ImageFx::ImageFxImpl::ResetCentres()
{
  m_images[0].centre = {200, 200};
  m_images[1].centre = {200, static_cast<int>(m_goomInfo->GetScreenInfo().height) - 200};
  m_images[2].centre = {static_cast<int>(m_goomInfo->GetScreenInfo().width) - 200,
                        static_cast<int>(m_goomInfo->GetScreenInfo().height) - 200};
  m_images[3].centre = {static_cast<int>(m_goomInfo->GetScreenInfo().width) - 200, 200};
}

void ImageFx::ImageFxImpl::UpdateCentres()
{
  for (auto& image : m_images)
  {
    const V2dInt start = GetNextStartingCentre(image);
    const auto x = static_cast<int>(
        stdnew::lerp(static_cast<float>(start.x), static_cast<float>(m_targetPos.x), m_tVal));
    const auto y = static_cast<int>(
        stdnew::lerp(static_cast<float>(start.y), static_cast<float>(m_targetPos.y), m_tVal));
    image.centre = {GetRandInRange(x - 20, x + 20), GetRandInRange(y - 20, y + 20)};
  }
}

auto ImageFx::ImageFxImpl::GetNextStartingCentre(Image& image) -> V2dInt
{
  const V2dInt coord = m_path[static_cast<size_t>(image.pathCounter)];
  image.pathCounter += image.backwards ? -image.counterStep : +image.counterStep;
  if (image.pathCounter <= 0)
  {
    image.pathCounter = 0;
    image.backwards = false;
  }
  if (static_cast<size_t>(image.pathCounter) >= m_path.size())
  {
    image.pathCounter = static_cast<int>(m_path.size() - 1);
    image.backwards = true;
  }
  return coord;
}

void ImageFx::ImageFxImpl::InitPath()
{
  const float angleStep = m_two_pi / static_cast<float>(m_path.size() - 1);
  const auto radius = static_cast<float>(m_goomInfo->GetScreenInfo().height / 2);
  float angle = 0.0;
  const auto x0 = static_cast<int32_t>(m_goomInfo->GetScreenInfo().width / 2);
  const auto y0 = static_cast<int32_t>(m_goomInfo->GetScreenInfo().height / 2);
  for (auto& coord : m_path)
  {
    coord = {x0 + static_cast<int>(radius * std::sin(angle)),
             y0 + static_cast<int>(radius * std::cos(angle))};
    angle += angleStep;
  }
}

} // namespace GOOM
