#ifndef VISUALIZATION_GOOM_TEXT_DRAW_H
#define VISUALIZATION_GOOM_TEXT_DRAW_H

#include <functional>
#include <memory>
#include <string>

namespace GOOM
{

class Pixel;
class PixelBuffer;

class TextDraw
{
public:
  TextDraw() noexcept;
  TextDraw(uint32_t screenWidth, uint32_t screenHeight) noexcept;
  ~TextDraw() noexcept;
  TextDraw(const TextDraw&) noexcept = delete;
  TextDraw(TextDraw&&) noexcept = delete;
  auto operator=(const TextDraw&) noexcept -> TextDraw& = delete;
  auto operator=(TextDraw&&) noexcept -> TextDraw& = delete;

  enum class TextAlignment
  {
    LEFT,
    CENTER,
    RIGHT
  };
  void SetAlignment(TextAlignment a);

  void SetFontSize(int val);
  void SetOutlineWidth(float val);
  void SetCharSpacing(float val);
  [[nodiscard]] auto GetFontFile() const -> const std::string&;
  void SetFontFile(const std::string& filename);
  void SetText(const std::string& str);

  using FontColorFunc =
      std::function<Pixel(size_t textIndexOfChar, int x, int y, int width, int height)>;
  void SetFontColorFunc(const FontColorFunc& f);
  void SetOutlineFontColorFunc(const FontColorFunc& f);

  void Prepare();

  struct Rect
  {
    int xmin{};
    int xmax{};
    int ymin{};
    int ymax{};
    [[nodiscard]] auto Width() const -> int { return xmax - xmin + 1; }
    [[nodiscard]] auto Height() const -> int { return ymax - ymin + 1; }
  };
  [[nodiscard]] auto GetPreparedTextBoundingRect() const -> Rect;
  [[nodiscard]] auto GetBearingX() const -> int;
  [[nodiscard]] auto GetBearingY() const -> int;

  void Draw(int xPen, int yPen, PixelBuffer& buff);
  void Draw(int xPen, int yPen, int& xNext, int& yNext, PixelBuffer& buff);

private:
  class TextDrawImpl;
  std::unique_ptr<TextDrawImpl> m_textDrawImpl;
};

} // namespace GOOM
#endif
