#ifndef VISUALIZATION_GOOM_LIB_GOOMUTILS_IMAGE_BITMAPS_H
#define VISUALIZATION_GOOM_LIB_GOOMUTILS_IMAGE_BITMAPS_H

#include "goom/goom_graphic.h"

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

class ImageBitmap : public PixelBuffer
{
public:
  ImageBitmap() noexcept = default;
  explicit ImageBitmap(std::string imageFilename);

  void Load(std::string imageFilename);

private:
  std::string m_filename{};
};

inline ImageBitmap::ImageBitmap(std::string imageFilename) : PixelBuffer{}
{
  Load(std::move(imageFilename));
}


#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
#endif
