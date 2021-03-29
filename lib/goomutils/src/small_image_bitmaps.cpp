#include "small_image_bitmaps.h"

#include "goom/goom_config.h"
#include "goomutils/logging_control.h"
//#undef NO_LOGGING
#include "goomutils/logging.h"

#include <array>
#include <format>
#include <string>
#include <utility>

#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

const std::array<std::string, static_cast<size_t>(SmallImageBitmaps::ImageNames::_SIZE)>
    SmallImageBitmaps::IMAGE_NAMES{
        "circle",
        "sphere",
        "yellow-flower",
        "red-flower",
    };

SmallImageBitmaps::SmallImageBitmaps(std::string resourcesDirectory)
  : m_resourcesDirectory{std::move(resourcesDirectory)}
{
  for (size_t res = MIN_IMAGE_SIZE; res <= MAX_IMAGE_SIZE; res += 2)
  {
    for (size_t i = 0; i < static_cast<size_t>(ImageNames::_SIZE); ++i)
    {
      const auto name = static_cast<ImageNames>(i);
      m_bitmapImages.emplace(GetImageKey(name, res), GetImageBitmapPtr(name, res));
      LogInfo("Loaded image bitmap: '{}'.", GetImageKey(name, res));
    }
  }
}

auto SmallImageBitmaps::GetImageBitmap(const ImageNames name, size_t res) const
    -> const ImageBitmap&
{
  LogDebug("Getting image bitmap: '{}'.", GetImageKey(name, res));
  return *m_bitmapImages.at(GetImageKey(name, res));
}

auto SmallImageBitmaps::GetImageBitmapPtr(const ImageNames name, const size_t sizeOfImageSquare)
    -> std::unique_ptr<const ImageBitmap>
{
  return std::make_unique<const ImageBitmap>(GetImageFilename(name, sizeOfImageSquare));
}

inline auto SmallImageBitmaps::GetImageKey(const ImageNames name, const size_t sizeOfImageSquare)
    -> std::string
{
  return std20::format("{}_{:02}", IMAGE_NAMES.at(static_cast<size_t>(name)), sizeOfImageSquare);
}

auto SmallImageBitmaps::GetImageFilename(const ImageNames name,
                                         const size_t sizeOfImageSquare) const -> std::string
{
  const std::string imagesDir = m_resourcesDirectory + PATH_SEP + IMAGES_DIR;
  std::string filename =
      std20::format("{}/{}{:02}x{:02}.png", imagesDir, IMAGE_NAMES.at(static_cast<size_t>(name)),
                    sizeOfImageSquare, sizeOfImageSquare);
  return filename;
}

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
