#include "draw_methods.h"

#include "goom_graphic.h"
#include "goomutils/colorutils.h"

#undef NDEBUG
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace GOOM
{

void DrawCircle(PixelBuffer& buff,
                const int x0,
                const int y0,
                const int radius,
                const Pixel& color,
                const uint32_t buffIntensity,
                const bool allowOverexposed,
                const uint32_t screenWidth,
                const uint32_t screenHeight)
{
  const std::vector<PixelBuffer*> buffs{&buff};
  const std::vector<Pixel> colors{color};
  DrawCircle(buffs, x0, y0, radius, colors, buffIntensity, allowOverexposed, screenWidth,
             screenHeight);
}

using PlotPoints = std::function<void(int x1, int y1, int x2, int y2)>;

// Function for circle-generation using Bresenham's algorithm
static void DrawBresenhamCircle(const int x0,
                                const int y0,
                                const int radius,
                                const PlotPoints& plotter)
{
  const auto drawCircle8 = [&](const int xc, int const yc, const int x, const int y) {
    plotter(xc - x, yc + y, xc + x, yc + y);
    plotter(xc - x, yc - y, xc + x, yc - y);
    plotter(xc - y, yc + x, xc + y, yc + x);
    plotter(xc - y, yc - x, xc + y, yc - x);
  };

  int x = 0;
  int y = radius;

  drawCircle8(x0, y0, x, y);

  int d = 3 - 2 * radius;
  while (y >= x)
  {
    x++;

    if (d > 0)
    {
      y--;
      d = d + 4 * (x - y) + 10;
    }
    else
    {
      d = d + 4 * x + 6;
    }
    drawCircle8(x0, y0, x, y);
  }
}

void DrawCircle(const std::vector<PixelBuffer*>& buffs,
                const int x0,
                const int y0,
                const int radius,
                const std::vector<Pixel>& colors,
                const uint32_t buffIntensity,
                const bool allowOverexposed,
                const uint32_t screenWidth,
                const uint32_t screenHeight)
{
  assert(buffs.size() == colors.size());

  auto plotter = [&](const int x1, const int y1, const int x2, const int y2) -> void {
    if (static_cast<uint32_t>(x1) >= screenWidth || static_cast<uint32_t>(y1) >= screenHeight)
    {
      return;
    }
    if (static_cast<uint32_t>(x2) >= screenWidth || static_cast<uint32_t>(y2) >= screenHeight)
    {
      return;
    }
    DrawPixels(buffs, x1, y1, colors, buffIntensity, allowOverexposed);
    if (x1 == x2 && y1 == y2)
    {
      return;
    }
    DrawPixels(buffs, x2, y2, colors, buffIntensity, allowOverexposed);
  };

  DrawBresenhamCircle(x0, y0, radius, plotter);
}

static void DrawHorizontalLine(const std::vector<PixelBuffer*>& buffs,
                               const int x1,
                               const int y,
                               const int x2,
                               const std::vector<Pixel>& colors,
                               const uint32_t buffIntensity,
                               const bool allowOverexposed)
{
  const int xEnd = x1 == x2 ? x1 : x2;
  for (int x = x1; x <= xEnd; x++)
  {
    DrawPixels(buffs, x, y, colors, buffIntensity, allowOverexposed);
  }
}

static void DrawWuLine(const std::vector<PixelBuffer*>& buffs,
                       int x1,
                       int y1,
                       int x2,
                       int y2,
                       const std::vector<Pixel>& colors,
                       uint32_t buffIntensity,
                       bool allowOverexposed,
                       uint32_t screenWidth,
                       uint32_t screenHeight);

void DrawFilledCircle(const std::vector<PixelBuffer*>& buffs,
                      const int x0,
                      const int y0,
                      const int radius,
                      const std::vector<Pixel>& colors,
                      const uint32_t buffIntensity,
                      const bool allowOverexposed,
                      [[maybe_unused]] const uint32_t screenHeight)
{
  assert(buffs.size() == colors.size());

  auto plotter = [&](const int x1, const int y1, const int x2, const int y2) -> void {
    assert(y1 == y2);
    DrawHorizontalLine(buffs, x1, y1, x2, colors, buffIntensity, allowOverexposed);
  };

  DrawBresenhamCircle(x0, y0, radius, plotter);
}

constexpr int LINE_THICKNESS_MIDDLE = 0;
constexpr int LINE_THICKNESS_DRAW_CLOCKWISE = 1;
constexpr int LINE_THICKNESS_DRAW_COUNTERCLOCKWISE = 2;

static void DrawThickLine(const std::vector<PixelBuffer*>& buffs,
                          int x0,
                          int y0,
                          int x1,
                          int y1,
                          const std::vector<Pixel>& colors,
                          uint32_t buffIntensity,
                          bool allowOverexposed,
                          uint8_t thickness,
                          uint8_t thicknessMode,
                          uint32_t screenWidth,
                          uint32_t screenHeight);

void DrawLine(PixelBuffer& buff,
              const int x1,
              const int y1,
              const int x2,
              const int y2,
              const Pixel& color,
              const uint32_t buffIntensity,
              const bool allowOverexposed,
              const uint8_t thickness,
              const uint32_t screenWidth,
              const uint32_t screenHeight)
{
  const std::vector<PixelBuffer*> buffs{&buff};
  const std::vector<Pixel> colors{color};

  DrawLine(buffs, x1, y1, x2, y2, colors, buffIntensity, allowOverexposed, thickness, screenWidth,
           screenHeight);
}

void DrawLine(const std::vector<PixelBuffer*>& buffs,
              int x1,
              int y1,
              int x2,
              int y2,
              const std::vector<Pixel>& colors,
              const uint32_t buffIntensity,
              const bool allowOverexposed,
              const uint8_t thickness,
              const uint32_t screenWidth,
              const uint32_t screenHeight)
{
  if (thickness == 1)
  {
    DrawWuLine(buffs, x1, y1, x2, y2, colors, buffIntensity, allowOverexposed, screenWidth,
               screenHeight);
  }
  else
  {
    DrawThickLine(buffs, x1, y1, x2, y2, colors, buffIntensity, allowOverexposed, thickness,
                  LINE_THICKNESS_MIDDLE, screenWidth, screenHeight);
    // plotLineWidth(n, buffs, colors, x1, y1, x2, y2, 1.0, screenx, screeny);
  }
}

using PlotFunc = const std::function<void(int x, int y, float brightess)>;
static void WuLine(float x0, float y0, float x1, float y1, const PlotFunc& plot);

static void DrawWuLine(const std::vector<PixelBuffer*>& buffs,
                       const int x1,
                       const int y1,
                       const int x2,
                       const int y2,
                       const std::vector<Pixel>& colors,
                       const uint32_t buffIntensity,
                       const bool allowOverexposed,
                       const uint32_t screenWidth,
                       const uint32_t screenHeight)
{
  if ((y1 < 0) || (y2 < 0) || (x1 < 0) || (x2 < 0) || (y1 >= static_cast<int>(screenHeight)) ||
      (y2 >= static_cast<int>(screenHeight)) || (x1 >= static_cast<int>(screenWidth)) ||
      (x2 >= static_cast<int>(screenWidth)))
  {
    return;
  }

  assert(buffs.size() == colors.size());
  std::vector<Pixel> tempColors = colors;
  auto plot = [&](const int x, const int y, const float brightness) -> void {
    if (static_cast<uint32_t>(x) >= screenWidth || static_cast<uint32_t>(y) >= screenHeight)
    {
      return;
    }
    constexpr float TOLERANCE = 0.001;
    if (brightness < TOLERANCE)
    {
      return;
    }
    if (brightness >= (1.0F - TOLERANCE))
    {
      DrawPixels(buffs, x, y, colors, buffIntensity, allowOverexposed);
    }
    else
    {
      for (size_t i = 0; i < colors.size(); i++)
      {
        tempColors[i] = UTILS::GetBrighterColor(brightness, colors[i], allowOverexposed);
      }
      DrawPixels(buffs, x, y, tempColors, buffIntensity, allowOverexposed);
    }
  };

  WuLine(static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2),
         static_cast<float>(y2), plot);
}

// The Xiaolin Wu anti-aliased draw line.
// From https://rosettacode.org/wiki/Xiaolin_Wu%27s_line_algorithm#C.2B.2B
//
static void WuLine(float x0, float y0, float x1, float y1, const PlotFunc& plot)
{
  const auto ipart = [](const float x) -> int { return static_cast<int>(std::floor(x)); };
  const auto round = [](const float x) -> float { return std::round(x); };
  const auto fpart = [](const float x) -> float { return x - std::floor(x); };
  const auto rfpart = [=](const float x) -> float { return 1 - fpart(x); };

  const bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
  if (steep)
  {
    std::swap(x0, y0);
    std::swap(x1, y1);
  }
  if (x0 > x1)
  {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  const float dx = x1 - x0; // because of above swap, must be >= 0
  const float dy = y1 - y0;
  const float gradient = (dx < 0.001) ? 1 : dy / dx;

  int xpx11;
  float intery;
  {
    const float xend = round(x0);
    const float yend = y0 + gradient * (xend - x0);
    const float xgap = rfpart(x0 + 0.5F);
    xpx11 = static_cast<int>(xend);
    const int ypx11 = ipart(yend);
    if (steep)
    {
      plot(ypx11, xpx11, rfpart(yend) * xgap);
      plot(ypx11 + 1, xpx11, fpart(yend) * xgap);
    }
    else
    {
      plot(xpx11, ypx11, rfpart(yend) * xgap);
      plot(xpx11, ypx11 + 1, fpart(yend) * xgap);
    }
    intery = yend + gradient;
  }

  int xpx12;
  {
    const float xend = round(x1);
    const float yend = y1 + gradient * (xend - x1);
    const float xgap = rfpart(x1 + 0.5F);
    xpx12 = static_cast<int>(xend);
    const int ypx12 = ipart(yend);
    if (steep)
    {
      plot(ypx12, xpx12, rfpart(yend) * xgap);
      plot(ypx12 + 1, xpx12, fpart(yend) * xgap);
    }
    else
    {
      plot(xpx12, ypx12, rfpart(yend) * xgap);
      plot(xpx12, ypx12 + 1, fpart(yend) * xgap);
    }
  }

  if (steep)
  {
    for (int x = xpx11 + 1; x < xpx12; x++)
    {
      plot(ipart(intery), x, rfpart(intery));
      plot(ipart(intery) + 1, x, fpart(intery));
      intery += gradient;
    }
  }
  else
  {
    for (int x = xpx11 + 1; x < xpx12; x++)
    {
      plot(x, ipart(intery), rfpart(intery));
      plot(x, ipart(intery) + 1, fpart(intery));
      intery += gradient;
    }
  }
}

/**
 * Modified Bresenham draw(line) with optional overlap. Required for drawThickLine().
 * Overlap draws additional pixel when changing minor direction. For standard bresenham overlap,
 * choose LINE_OVERLAP_NONE (0).
 *
 *  Sample line:
 *
 *    00+
 *     -0000+
 *         -0000+
 *             -00
 *
 *  0 pixels are drawn for normal line without any overlap
 *  + pixels are drawn if LINE_OVERLAP_MAJOR
 *  - pixels are drawn if LINE_OVERLAP_MINOR
 */

constexpr int LINE_OVERLAP_NONE = 0;
constexpr int LINE_OVERLAP_MAJOR = 1;
constexpr int LINE_OVERLAP_MINOR = 2;

static void DrawLineOverlap(const std::vector<PixelBuffer*>& buffs,
                            int x0,
                            int y0,
                            int x1,
                            int y1,
                            const std::vector<Pixel>& colors,
                            const uint32_t buffIntensity,
                            const bool allowOverexposed,
                            const float brightness,
                            const uint8_t overlap,
                            const uint32_t screenWidth,
                            const uint32_t screenHeight)
{
  if ((y0 < 0) || (y1 < 0) || (x0 < 0) || (x1 < 0) || (y0 >= static_cast<int>(screenHeight)) ||
      (y1 >= static_cast<int>(screenHeight)) || (x0 >= static_cast<int>(screenWidth)) ||
      (x1 >= static_cast<int>(screenWidth)))
  {
    return;
  }

  assert(buffs.size() == colors.size());
  std::vector<Pixel> tempColors = colors;
  auto plot = [&](const int x, const int y) -> void {
    if (static_cast<uint32_t>(x) >= screenWidth || static_cast<uint32_t>(y) >= screenHeight)
    {
      return;
    }
    constexpr float TOLERANCE = 0.001;
    if (brightness >= (1.0 - TOLERANCE))
    {
      DrawPixels(buffs, x, y, colors, buffIntensity, allowOverexposed);
    }
    else
    {
      for (size_t i = 0; i < colors.size(); i++)
      {
        tempColors[i] = UTILS::GetBrighterColor(brightness, colors[i], allowOverexposed);
      }
      DrawPixels(buffs, x, y, tempColors, buffIntensity, allowOverexposed);
    }
  };

  if ((x0 == x1) || (y0 == y1))
  {
    //horizontal or vertical line -> fillRect() is faster than drawLine()
    //        LocalDisplay.fillRect(aXStart, aYStart, aXEnd, aYEnd, aColor);
    // ????????????????????????????????????????????????????????????????????????????????????????????
    DrawWuLine(buffs, x0, y0, x1, y1, colors, buffIntensity, allowOverexposed, screenWidth,
               screenHeight);
  }
  else
  {
    int16_t error;
    int16_t stepX;
    int16_t stepY;

    // Calculate direction.
    auto deltaX = static_cast<int16_t>(x1 - x0);
    auto deltaY = static_cast<int16_t>(y1 - y0);
    if (deltaX < 0)
    {
      deltaX = -deltaX;
      stepX = -1;
    }
    else
    {
      stepX = +1;
    }
    if (deltaY < 0)
    {
      deltaY = -deltaY;
      stepY = -1;
    }
    else
    {
      stepY = +1;
    }

    const int16_t deltaXTimes2 = deltaX << 1;
    const int16_t deltaYTimes2 = deltaY << 1;

    // Draw start pixel.
    plot(x0, y0);
    if (deltaX > deltaY)
    {
      // Start value represents a half step in Y direction.
      error = deltaYTimes2 - deltaX;
      while (x0 != x1)
      {
        // Step in main direction.
        x0 += stepX;
        if (error >= 0)
        {
          if (overlap == LINE_OVERLAP_MAJOR)
          {
            // Draw pixel in main direction before changing.
            plot(x0, y0);
          }
          // change Y
          y0 += stepY;
          if (overlap == LINE_OVERLAP_MINOR)
          {
            // Draw pixel in minor direction before changing.
            plot(x0 - stepX, y0);
          }
          error -= deltaXTimes2;
        }
        error += deltaYTimes2;
        plot(x0, y0);
      }
    }
    else
    {
      error = deltaXTimes2 - deltaY;
      while (y0 != y1)
      {
        y0 += stepY;
        if (error >= 0)
        {
          if (overlap == LINE_OVERLAP_MAJOR)
          {
            // Draw pixel in main direction before changing.
            plot(x0, y0);
          }
          x0 += stepX;
          if (overlap == LINE_OVERLAP_MINOR)
          {
            // Draw pixel in minor direction before changing.
            plot(x0, y0 - stepY);
          }
          error -= deltaYTimes2;
        }
        error += deltaXTimes2;
        plot(x0, y0);
      }
    }
  }
}

/**
 * Bresenham with thickness.
 * No pixel missed and every pixel only drawn once!
 * thicknessMode can be one of LINE_THICKNESS_MIDDLE, LINE_THICKNESS_DRAW_CLOCKWISE,
 *   LINE_THICKNESS_DRAW_COUNTERCLOCKWISE
 */

static void DrawThickLine(const std::vector<PixelBuffer*>& buffs,
                          int x0,
                          int y0,
                          int x1,
                          int y1,
                          const std::vector<Pixel>& colors,
                          const uint32_t buffIntensity,
                          const bool allowOverexposed,
                          const uint8_t thickness,
                          const uint8_t thicknessMode,
                          const uint32_t screenWidth,
                          const uint32_t screenHeight)
{
  if ((y0 < 0) || (y1 < 0) || (x0 < 0) || (x1 < 0) || (y0 >= static_cast<int>(screenHeight)) ||
      (y1 >= static_cast<int>(screenHeight)) || (x0 >= static_cast<int>(screenWidth)) ||
      (x1 >= static_cast<int>(screenWidth)))
  {
    return;
  }

  if (thickness <= 1)
  {
    DrawLineOverlap(buffs, x0, y0, x1, y1, colors, buffIntensity, allowOverexposed, 1.0, 0,
                    screenWidth, screenHeight);
  }

  const float brightness = 0.8F * 2.0F / static_cast<float>(thickness);

  /**
    * For coordinate system with 0.0 top left
    * Swap X and Y delta and calculate clockwise (new delta X inverted)
    * or counterclockwise (new delta Y inverted) rectangular direction.
    * The right rectangular direction for LINE_OVERLAP_MAJOR toggles with each octant.
  */

  int16_t error;
  int16_t stepX;
  int16_t stepY;
  auto deltaY = static_cast<int16_t>(x1 - x0);
  auto deltaX = static_cast<int16_t>(y1 - y0);

  // Mirror 4 quadrants to one and adjust deltas and stepping direction.
  bool swap = true; // count effective mirroring
  if (deltaX < 0)
  {
    deltaX = -deltaX;
    stepX = -1;
    swap = !swap;
  }
  else
  {
    stepX = +1;
  }
  if (deltaY < 0)
  {
    deltaY = -deltaY;
    stepY = -1;
    swap = !swap;
  }
  else
  {
    stepY = +1;
  }

  const int deltaXTimes2 = deltaX << 1;
  const int deltaYTimes2 = deltaY << 1;

  bool overlap;
  // Adjust for right direction of thickness from line origin.
  int drawStartAdjustCount = thickness / 2;
  if (thicknessMode == LINE_THICKNESS_DRAW_COUNTERCLOCKWISE)
  {
    drawStartAdjustCount = thickness - 1;
  }
  else if (thicknessMode == LINE_THICKNESS_DRAW_CLOCKWISE)
  {
    drawStartAdjustCount = 0;
  }

  // Which octant are we now?
  if (deltaX >= deltaY)
  {
    if (swap)
    {
      drawStartAdjustCount = (thickness - 1) - drawStartAdjustCount;
      stepY = -stepY;
    }
    else
    {
      stepX = -stepX;
    }
    /*
     * Vector for draw direction of start of lines is rectangular and counterclockwise to
     * main line direction. Therefore no pixel will be missed if LINE_OVERLAP_MAJOR is used
     * on change in minor rectangular direction.
     */
    // adjust draw start point
    error = deltaYTimes2 - deltaX;
    for (int i = drawStartAdjustCount; i > 0; i--)
    {
      // change X (main direction here)
      x0 -= stepX;
      x1 -= stepX;
      if (error >= 0)
      {
        // change Y
        y0 -= stepY;
        y1 -= stepY;
        error -= deltaXTimes2;
      }
      error += deltaYTimes2;
    }
    //draw start line
    DrawLineOverlap(buffs, x0, y0, x1, y1, colors, buffIntensity, allowOverexposed, brightness, 1,
                    screenWidth, screenHeight);
    // draw 'thickness' number of lines
    error = deltaYTimes2 - deltaX;
    for (int i = thickness; i > 1; i--)
    {
      // change X (main direction here)
      x0 += stepX;
      x1 += stepX;
      overlap = LINE_OVERLAP_NONE;
      if (error >= 0)
      {
        // change Y
        y0 += stepY;
        y1 += stepY;
        error -= deltaXTimes2;
        /*
         * Change minor direction reverse to line (main) direction because of choosing
         * the right (counter)clockwise draw vector. Use LINE_OVERLAP_MAJOR to fill all pixels.
         *
         * EXAMPLE:
         * 1,2 = Pixel of first 2 lines
         * 3 = Pixel of third line in normal line mode
         * - = Pixel which will additionally be drawn in LINE_OVERLAP_MAJOR mode
         *           33
         *       3333-22
         *   3333-222211
         *   33-22221111
         *  221111                     /\
         *  11                          Main direction of start of lines draw vector
         *  -> Line main direction
         *  <- Minor direction of counterclockwise of start of lines draw vector
         */
        overlap = LINE_OVERLAP_MAJOR;
      }
      error += deltaYTimes2;
      DrawLineOverlap(buffs, x0, y0, x1, y1, colors, buffIntensity, allowOverexposed, brightness,
                      overlap, screenWidth, screenHeight);
    }
  }
  else
  {
    // the other octant
    if (swap)
    {
      stepX = -stepX;
    }
    else
    {
      drawStartAdjustCount = (thickness - 1) - drawStartAdjustCount;
      stepY = -stepY;
    }
    // adjust draw start point
    error = deltaXTimes2 - deltaY;
    for (int i = drawStartAdjustCount; i > 0; i--)
    {
      y0 -= stepY;
      y1 -= stepY;
      if (error >= 0)
      {
        x0 -= stepX;
        x1 -= stepX;
        error -= deltaYTimes2;
      }
      error += deltaXTimes2;
    }
    // draw start line
    DrawLineOverlap(buffs, x0, y0, x1, y1, colors, buffIntensity, allowOverexposed, brightness, 0,
                    screenWidth, screenHeight);
    // draw 'thickness' number of lines
    error = deltaXTimes2 - deltaY;
    for (int i = thickness; i > 1; i--)
    {
      y0 += stepY;
      y1 += stepY;
      overlap = LINE_OVERLAP_NONE;
      if (error >= 0)
      {
        x0 += stepX;
        x1 += stepX;
        error -= deltaYTimes2;
        overlap = LINE_OVERLAP_MAJOR;
      }
      error += deltaXTimes2;
      DrawLineOverlap(buffs, x0, y0, x1, y1, colors, buffIntensity, allowOverexposed, brightness,
                      overlap, screenWidth, screenHeight);
    }
  }
}

} // namespace GOOM
