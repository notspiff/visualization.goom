#ifndef VISUALIZATION_GOOM_GOOM_DRAW_H
#define VISUALIZATION_GOOM_GOOM_DRAW_H

#include "goom_config.h"
#include "goom_graphic.h"
#include "goomutils/parallel_utils.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace GOOM
{

class GoomDraw
{
public:
  GoomDraw(uint32_t screenWidth, uint32_t screenHeight);

  [[nodiscard]] auto GetAllowOverexposed() const -> bool;
  void SetAllowOverexposed(bool val);

  [[nodiscard]] auto GetBuffIntensity() const -> float;
  void SetBuffIntensity(float val);

  [[nodiscard]] static auto GetPixelRgb(const PixelBuffer& buff, uint32_t x, uint32_t y) -> Pixel;
  void SetPixelRgb(PixelBuffer& buff, uint32_t x, uint32_t y, const Pixel& color) const;
  // Set the pixel but don't blend it with the existing pixel value.
  static void SetPixelRgbNoBlend(PixelBuffer& buff, uint32_t x, uint32_t y, const Pixel& color);

  [[nodiscard]] auto GetPixelRgb(const std::vector<PixelBuffer*>& buffs,
                                 uint32_t x,
                                 uint32_t y) const -> std::vector<Pixel>;
  void SetPixelRgb(const std::vector<PixelBuffer*>& buff,
                   uint32_t x,
                   uint32_t y,
                   const std::vector<Pixel>& colors) const;

  void Circle(PixelBuffer&, int x0, int y0, int radius, const Pixel& color) const;

  void Circle(const std::vector<PixelBuffer*>& buffs,
              int x0,
              int y0,
              int radius,
              const std::vector<Pixel>& colors) const;

  using GetBitmapColorFunc = std::function<auto(size_t x, size_t y, const Pixel& b)->Pixel>;

  void Bitmap(PixelBuffer& buff,
              int xCentre,
              int yCentre,
              const PixelBuffer& bitmap,
              const GetBitmapColorFunc& getColor,
              uint32_t xStride = 1,
              uint32_t yStride = 1);

  void Bitmap(const std::vector<PixelBuffer*>& buffs,
              int xCentre,
              int yCentre,
              const std::vector<const PixelBuffer*>& bitmaps,
              const std::vector<GetBitmapColorFunc>& getColors,
              uint32_t xStride = 1,
              uint32_t yStride = 1);

  void Line(
      PixelBuffer&, int x1, int y1, int x2, int y2, const Pixel& color, uint8_t thickness) const;

  void Line(const std::vector<PixelBuffer*>& buffs,
            int x1,
            int y1,
            int x2,
            int y2,
            const std::vector<Pixel>& colors,
            uint8_t thickness) const;

private:
  const uint32_t m_screenWidth;
  const uint32_t m_screenHeight;
  bool m_allowOverexposed = false;
  float m_buffIntensity = 1.0;
  uint32_t m_intBuffIntensity = channel_limits<uint32_t>::max();
  GOOM::UTILS::Parallel m_parallel{};
};

inline void GoomDraw::SetPixelRgbNoBlend(PixelBuffer& buff,
                                         const uint32_t x,
                                         const uint32_t y,
                                         const Pixel& color)
{
  buff(x, y) = color;
}

inline auto GoomDraw::GetPixelRgb(const PixelBuffer& buff, const uint32_t x, const uint32_t y)
    -> Pixel
{
  return buff(x, y);
}

inline auto GoomDraw::GetPixelRgb(const std::vector<PixelBuffer*>& buffs,
                                  const uint32_t x,
                                  const uint32_t y) const -> std::vector<Pixel>
{
  std::vector<Pixel> colors(buffs.size());
  for (size_t i = 0; i < buffs.size(); i++)
  {
    colors[i] = (*buffs[i])(x, y);
  }
  return colors;
}

[[maybe_unused]] inline auto GoomDraw::GetAllowOverexposed() const -> bool
{
  return m_allowOverexposed;
}

inline void GoomDraw::SetAllowOverexposed(const bool val)
{
  m_allowOverexposed = val;
}

inline auto GoomDraw::GetBuffIntensity() const -> float
{
  return m_buffIntensity;
}

inline void GoomDraw::SetBuffIntensity(const float val)
{
  m_buffIntensity = val;
  m_intBuffIntensity = static_cast<uint32_t>(channel_limits<float>::max() * m_buffIntensity);
}

} // namespace GOOM

#endif /* VISUALIZATION_GOOM_GOOM_DRAW_H */
