#include "goom_draw.h"

#include "draw_methods.h"
#include "goom_graphic.h"

#include <cstdint>
#include <vector>

namespace GOOM
{

using namespace UTILS;

GoomDraw::GoomDraw(const uint32_t screenW, const uint32_t screenH)
  : m_screenWidth{screenW}, m_screenHeight{screenH}, m_parallel{-1} // max cores - 1
{
  SetBuffIntensity(0.5);
  SetAllowOverexposed(true);
}

void GoomDraw::Circle(
    PixelBuffer& buff, const int x0, const int y0, const int radius, const Pixel& color) const
{
  DrawCircle(buff, x0, y0, radius, color, m_intBuffIntensity, m_allowOverexposed, m_screenWidth,
             m_screenHeight);
}

void GoomDraw::Circle(const std::vector<PixelBuffer*>& buffs,
                      const int x0,
                      const int y0,
                      const int radius,
                      const std::vector<Pixel>& colors) const
{
  DrawCircle(buffs, x0, y0, radius, colors, m_intBuffIntensity, m_allowOverexposed, m_screenWidth,
             m_screenHeight);
}

void GoomDraw::Bitmap(PixelBuffer& buff,
                      const int xCentre,
                      const int yCentre,
                      const PixelBuffer& bitmap,
                      const GetBitmapColorFunc& getColor,
                      const uint32_t xStride,
                      const uint32_t yStride)
{
  const auto bitmapWidth = static_cast<int>(bitmap.GetWidth());
  const auto bitmapHeight = static_cast<int>(bitmap.GetHeight());

  int x0 = xCentre - bitmapWidth / 2;
  int y0 = yCentre - bitmapHeight / 2;
  int x1 = x0 + bitmapWidth - 1;
  int y1 = y0 + bitmapHeight - 1;

  if (x0 >= static_cast<int>(m_screenWidth) || y0 >= static_cast<int>(m_screenHeight) || x1 < 0 ||
      y1 < 0)
  {
    return;
  }
  if (x0 < 0)
  {
    x0 = 0;
  }
  if (y0 < 0)
  {
    y0 = 0;
  }
  if (x1 >= static_cast<int>(m_screenWidth))
  {
    x1 = static_cast<int>(m_screenWidth - 1);
  }
  if (y1 >= static_cast<int>(m_screenHeight))
  {
    y1 = static_cast<int>(m_screenHeight - 1);
  }

  const size_t actualBitmapWidth = static_cast<size_t>(x1 - x0) + 1;
  const size_t actualBitmapHeight = static_cast<size_t>(y1 - y0) + 1;

  const auto setDestPixelRow = [&](const size_t yBitmap) {
    const int yBuff = y0 + static_cast<int>(yBitmap);
    for (size_t xBitmap = 0; xBitmap < actualBitmapWidth; xBitmap += xStride)
    {
      const int xBuff = x0 + static_cast<int>(xBitmap);
      const Pixel finalColor = getColor(xBitmap, yBitmap, bitmap(xBitmap, yBitmap));
      DrawPixel(&buff, xBuff, yBuff, finalColor, m_intBuffIntensity, m_allowOverexposed);
    }
  };

  if (bitmapWidth > 199)
  {
    m_parallel.ForLoop(actualBitmapHeight, setDestPixelRow);
  }
  else
  {
    for (size_t yBitmap = 0; yBitmap < actualBitmapHeight; yBitmap += yStride)
    {
      setDestPixelRow(yBitmap);
    }
  }
}

void GoomDraw::Bitmap(const std::vector<PixelBuffer*>& buffs,
                      const int xCentre,
                      const int yCentre,
                      const std::vector<const PixelBuffer*>& bitmaps,
                      const std::vector<GetBitmapColorFunc>& getColors,
                      const uint32_t xStride,
                      const uint32_t yStride)
{
  for (size_t i = 0; i < buffs.size(); i++)
  {
    Bitmap(*(buffs[i]), xCentre, yCentre, (*bitmaps[i]), getColors[i], xStride, yStride);
  }
}

void GoomDraw::Line(PixelBuffer& buff,
                    const int x1,
                    const int y1,
                    const int x2,
                    const int y2,
                    const Pixel& color,
                    const uint8_t thickness) const
{
  DrawLine(buff, x1, y1, x2, y2, color, m_intBuffIntensity, m_allowOverexposed, thickness,
           m_screenWidth, m_screenHeight);
}

void GoomDraw::Line(const std::vector<PixelBuffer*>& buffs,
                    int x1,
                    int y1,
                    int x2,
                    int y2,
                    const std::vector<Pixel>& colors,
                    const uint8_t thickness) const
{
  DrawLine(buffs, x1, y1, x2, y2, colors, m_intBuffIntensity, m_allowOverexposed, thickness,
           m_screenWidth, m_screenHeight);
}

void GoomDraw::SetPixelRgb(PixelBuffer& buff,
                           const uint32_t x,
                           const uint32_t y,
                           const Pixel& color) const
{
  const std::vector<PixelBuffer*> buffs{&buff};
  std::vector<Pixel> colors{color};
  DrawPixels(buffs, static_cast<int>(x), static_cast<int>(y), colors, m_intBuffIntensity,
             m_allowOverexposed);
}

void GoomDraw::SetPixelRgb(const std::vector<PixelBuffer*>& buffs,
                           const uint32_t x,
                           const uint32_t y,
                           const std::vector<Pixel>& colors) const
{
  DrawPixels(buffs, static_cast<int>(x), static_cast<int>(y), colors, m_intBuffIntensity,
             m_allowOverexposed);
}

} // namespace GOOM
