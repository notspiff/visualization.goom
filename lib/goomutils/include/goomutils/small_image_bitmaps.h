#ifndef VISUALIZATION_GOOM_SMALL_IMAGE_BITMAPS_H
#define VISUALIZATION_GOOM_SMALL_IMAGE_BITMAPS_H

#include "goomutils/image_bitmaps.h"

#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <string>

#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

class SmallImageBitmaps
{
public:
  enum class ImageNames
  {
    CIRCLE,
    SPHERE,
    YELLOW_FLOWER,
    RED_FLOWER,
    _SIZE
  };
  static constexpr size_t MIN_IMAGE_SIZE = 3;
  static constexpr size_t MAX_IMAGE_SIZE = 21;

  explicit SmallImageBitmaps(std::string resourcesDirectory);
  auto GetImageBitmap(ImageNames name, size_t res) const -> const ImageBitmap&;
  // void AddImageBitmap(const std::string& name, size_t res);

private:
  static const std::array<std::string, static_cast<size_t>(ImageNames::_SIZE)> IMAGE_NAMES;
  std::string m_resourcesDirectory;
  std::map<std::string, std::unique_ptr<const ImageBitmap>> m_bitmapImages{};
  auto GetImageBitmapPtr(ImageNames name, size_t sizeOfImageSquare)
      -> std::unique_ptr<const ImageBitmap>;
  static auto GetImageKey(ImageNames name, size_t sizeOfImageSquare) -> std::string;
  auto GetImageFilename(ImageNames name, size_t sizeOfImageSquare) const -> std::string;
};

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
#endif //VISUALIZATION_GOOM_SMALL_IMAGE_BITMAPS_H
