#ifndef VISUALIZATION_GOOM_DRAW_METHODS_H
#define VISUALIZATION_GOOM_DRAW_METHODS_H

#include "goom_graphic.h"
#include "goomutils/colorutils.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace GOOM
{

void DrawCircle(PixelBuffer& buff,
                int x0,
                int y0,
                int radius,
                const Pixel& color,
                uint32_t buffIntensity,
                bool allowOverexposed,
                uint32_t screenWidth,
                uint32_t screenHeight);

void DrawCircle(const std::vector<PixelBuffer*>& buffs,
                int x0,
                int y0,
                int radius,
                const std::vector<Pixel>& colors,
                uint32_t buffIntensity,
                bool allowOverexposed,
                uint32_t screenWidth,
                uint32_t screenHeight);

void DrawLine(const std::vector<PixelBuffer*>& buffs,
              int x1,
              int y1,
              int x2,
              int y2,
              const std::vector<Pixel>& colors,
              uint32_t buffIntensity,
              bool allowOverexposed,
              uint8_t thickness,
              uint32_t screenWidth,
              uint32_t screenHeight);

void DrawLine(PixelBuffer& buff,
              int x1,
              int y1,
              int x2,
              int y2,
              const Pixel& color,
              uint32_t buffIntensity,
              bool allowOverexposed,
              uint8_t thickness,
              uint32_t screenWidth,
              uint32_t screenHeight);

inline void DrawPixel(PixelBuffer* const buff,
                      const int x,
                      const int y,
                      const Pixel& newColor,
                      const uint32_t buffIntensity,
                      const bool allowOverexposed)
{
  const Pixel brighterPixColor =
      UTILS::GetBrighterColorInt(buffIntensity, newColor, allowOverexposed);
  Pixel& p = (*buff)(static_cast<size_t>(x), static_cast<size_t>(y));
  p = UTILS::GetColorAdd(p, brighterPixColor, allowOverexposed);

  /***
    ATTEMPT AT BLENDING - WON'T WORK THOUGH - BECAUSE OF MULTIPLE BUFFERS??
      Pixel* const p = &(buffs[i][pos]);
      const Pixel existingColorBlended =
          GetBrighterColorInt(buffIntensity, *p, allowOverexposed);
      const Pixel pixColorBlended =
          GetBrighterColorInt(channel_limits<uint32_t>::max() - buffIntensity, newColors[i],
                              allowOverexposed);
      *p = GetColorAdd(existingColorBlended, pixColorBlended, allowOverexposed);
    ***/
  }

  inline void DrawPixels(const std::vector<PixelBuffer*>& buffs,
                         const int x,
                         const int y,
                         const std::vector<Pixel>& newColors,
                         const uint32_t buffIntensity,
                         const bool allowOverexposed)
  {
    for (size_t i = 0; i < buffs.size(); i++)
    {
      DrawPixel(buffs[i], x, y, newColors[i], buffIntensity, allowOverexposed);
    }
  }

  } // namespace GOOM
#endif
