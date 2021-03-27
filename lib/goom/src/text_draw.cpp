#include "text_draw.h"

#include "goom_graphic.h"
#include "goomutils/colorutils.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <format>
#include <fstream>

namespace GOOM
{

class TextDraw::TextDrawImpl
{
public:
  TextDrawImpl(uint32_t screenW, uint32_t screenH) noexcept;
  ~TextDrawImpl() noexcept;
  TextDrawImpl(const TextDrawImpl&) noexcept = delete;
  TextDrawImpl(TextDrawImpl&&) noexcept = delete;
  auto operator=(const TextDrawImpl&) -> TextDrawImpl& = delete;
  auto operator=(TextDrawImpl&&) -> TextDrawImpl& = delete;

  void SetAlignment(TextAlignment a);
  [[nodiscard]] auto GetFontFile() const -> const std::string&;
  void SetFontFile(const std::string& filename);
  void SetFontSize(int val);
  void SetOutlineWidth(float val);
  void SetCharSpacing(float val);
  void SetText(const std::string& str);

  void SetFontColorFunc(const FontColorFunc& f);
  void SetOutlineFontColorFunc(const FontColorFunc& f);

  void Prepare();
  [[nodiscard]] auto GetPreparedTextBoundingRect() const -> Rect;
  [[nodiscard]] auto GetBearingX() const -> int;
  [[nodiscard]] auto GetBearingY() const -> int;

  void Draw(int xPen, int yPen, PixelBuffer& buff);
  void Draw(int xPen, int yPen, int& xNext, int& yNext, PixelBuffer& buff);

private:
  const int m_screenWidth;
  const int m_screenHeight;
  FT_Library m_library{};
  int m_fontSize = 100;
  uint32_t m_horizontalResolution = 90;
  uint32_t m_verticalResolution = 90;
  float m_outlineWidth = 3.0;
  float m_charSpacing = 0.0;
  std::string m_fontFilename{};
  std::vector<unsigned char> m_fontBuffer{};
  std::string m_theText{};
  TextAlignment m_textAlignment{TextAlignment::LEFT};
  FT_Face m_face{};
  void SetFaceFontSize();

  FontColorFunc m_getFontColor{};
  FontColorFunc m_getOutlineFontColor{};

  // clang compiler doesn't like 'static constexpr'
  /*static constexpr*/ static auto ToStdPixelCoord(int freeTypeCoord) -> int;
  /*static constexpr*/ static auto ToFreeTypeCoord(int stdPixelCoord) -> int;
  /*static constexpr*/ static auto ToFreeTypeCoord(float stdPixelCoord) -> int;
  struct Vec2;
  struct Span;
  using SpanArray = std::vector<Span>;

  struct RectImpl : public Rect
  {
    RectImpl() noexcept = default;
    ~RectImpl() noexcept = default;
    RectImpl(int left, int top, int right, int bottom) noexcept;
    RectImpl(const RectImpl&) noexcept = default;
    RectImpl(RectImpl&&) noexcept = default;
    auto operator=(const RectImpl&) noexcept -> RectImpl = delete;
    auto operator=(RectImpl&&) noexcept -> RectImpl = delete;

    void Include(const Vec2& span);
  };

  struct Spans
  {
    SpanArray stdSpans{};
    SpanArray outlineSpans{};
    size_t textIndexOfChar{};
    RectImpl rect{};
    int advance{};
    int bearingX{};
    int bearingY{};
  };

  std::vector<Spans> m_textSpans{};
  Rect m_textBoundingRect{};
  static auto GetBoundingRect(const SpanArray& stdSpans, const SpanArray& outlineSpans) -> RectImpl;
  [[nodiscard]] auto GetSpans(size_t textIndexOfChar) const -> Spans;
  [[nodiscard]] auto GetStdSpans() const -> SpanArray;
  [[nodiscard]] auto GetOutlineSpans() const -> SpanArray;
  void RenderSpans(FT_Outline* outline, SpanArray* spans) const;
  static void RasterCallback(int y, int count, const FT_Span* spans, void* user);

  [[nodiscard]] auto GetStartXPen(int xPen) const -> int;
  [[nodiscard]] static auto GetStartYPen(int yPen) -> int;
  void WriteGlyph(const Spans& s, int xPen, int yPen, PixelBuffer& buff) const;
  void WriteSpansToImage(const SpanArray& s,
                         const RectImpl& r,
                         int xPen,
                         int yPen,
                         size_t textIndexOfChar,
                         const FontColorFunc& getColor,
                         PixelBuffer& buff) const;
  //void WriteGlyphSimple(wchar_t ch, int xPen, int yPen, int& advance, PixelBuffer& buff);
  //void DrawBitmap(FT_Bitmap* bitmap, FT_Int x, FT_Int y, wchar_t ch, PixelBuffer& buff) const;
};

TextDraw::TextDraw() noexcept : m_textDrawImpl{}
{
}

TextDraw::TextDraw(const uint32_t screenW, const uint32_t screenH) noexcept
  : m_textDrawImpl{new TextDrawImpl{screenW, screenH}}
{
}

TextDraw::~TextDraw() noexcept = default;

void TextDraw::SetAlignment(TextAlignment a)
{
  m_textDrawImpl->SetAlignment(a);
}

auto TextDraw::GetFontFile() const -> const std::string&
{
  return m_textDrawImpl->GetFontFile();
}

void TextDraw::SetFontFile(const std::string& filename)
{
  m_textDrawImpl->SetFontFile(filename);
}

void TextDraw::SetFontSize(const int val)
{
  m_textDrawImpl->SetFontSize(val);
}

void TextDraw::SetOutlineWidth(const float val)
{
  m_textDrawImpl->SetOutlineWidth(val);
}

void TextDraw::SetCharSpacing(const float val)
{
  m_textDrawImpl->SetCharSpacing(val);
}

void TextDraw::SetText(const std::string& str)
{
  m_textDrawImpl->SetText(str);
}

void TextDraw::SetFontColorFunc(const FontColorFunc& f)
{
  m_textDrawImpl->SetFontColorFunc(f);
}

void TextDraw::SetOutlineFontColorFunc(const FontColorFunc& f)
{
  m_textDrawImpl->SetOutlineFontColorFunc(f);
}

void TextDraw::Prepare()
{
  m_textDrawImpl->Prepare();
}

auto TextDraw::GetPreparedTextBoundingRect() const -> TextDraw::Rect
{
  return m_textDrawImpl->GetPreparedTextBoundingRect();
}

auto TextDraw::GetBearingX() const -> int
{
  return m_textDrawImpl->GetBearingX();
}

auto TextDraw::GetBearingY() const -> int
{
  return m_textDrawImpl->GetBearingY();
}

void TextDraw::Draw(int xPen, int yPen, PixelBuffer& buff)
{
  m_textDrawImpl->Draw(xPen, yPen, buff);
}

void TextDraw::Draw(int xPen, int yPen, int& xNext, int& yNext, PixelBuffer& buff)
{
  m_textDrawImpl->Draw(xPen, yPen, xNext, yNext, buff);
}

TextDraw::TextDrawImpl::TextDrawImpl(const uint32_t screenW, const uint32_t screenH) noexcept
  : m_screenWidth{static_cast<int>(screenW)}, m_screenHeight{static_cast<int>(screenH)}
{
  (void)FT_Init_FreeType(&m_library);
}

TextDraw::TextDrawImpl::~TextDrawImpl() noexcept
{
  (void)FT_Done_FreeType(m_library);
}

void TextDraw::TextDrawImpl::SetAlignment(const TextAlignment a)
{
  m_textAlignment = a;
}

auto TextDraw::TextDrawImpl::GetFontFile() const -> const std::string&
{
  return m_fontFilename;
}

void TextDraw::TextDrawImpl::SetFontFile(const std::string& filename)
{
  m_fontFilename = filename;

  std::ifstream fontFile(m_fontFilename, std::ios::binary);
  if (!fontFile)
  {
    throw std::runtime_error(std20::format("Could not open font file \"{}\".", m_fontFilename));
  }

  (void)fontFile.seekg(0, std::ios::end);
  std::fstream::pos_type fontFileSize = fontFile.tellg();
  (void)fontFile.seekg(0);
  m_fontBuffer.resize(static_cast<size_t>(fontFileSize));
  (void)fontFile.read(reinterpret_cast<char*>(m_fontBuffer.data()), fontFileSize);

  // Create a face from a memory buffer.  Be sure not to delete the memory buffer
  // until we are done using that font as FreeType will reference it directly.
  (void)FT_New_Memory_Face(m_library, m_fontBuffer.data(),
                           static_cast<FT_Long>(m_fontBuffer.size()), 0, &m_face);

  SetFaceFontSize();
}

void TextDraw::TextDrawImpl::SetFaceFontSize()
{
  if (FT_Set_Char_Size(m_face, ToFreeTypeCoord(m_fontSize), ToFreeTypeCoord(m_fontSize),
                       m_horizontalResolution, m_verticalResolution) != 0)
  {
    throw std::logic_error(std20::format("Could not set face font size to {}.", m_fontSize));
  }
}

void TextDraw::TextDrawImpl::SetFontSize(const int val)
{
  if (val <= 0)
  {
    throw std::logic_error(std20::format("Font size <= 0: {}.", val));
  }

  m_fontSize = val;
  if (m_face)
  {
    SetFaceFontSize();
  }
}

void TextDraw::TextDrawImpl::SetOutlineWidth(const float val)
{
  if (val <= 0.0)
  {
    throw std::logic_error(std20::format("Outline width <= 0: {}.", val));
  }
  m_outlineWidth = val;
}

void TextDraw::TextDrawImpl::SetCharSpacing(const float val)
{
  if (val < 0.0)
  {
    throw std::logic_error(std20::format("Char spacing < 0: {}.", val));
  }
  m_charSpacing = val;
}

void TextDraw::TextDrawImpl::SetText(const std::string& str)
{
  if (str.empty())
  {
    throw std::logic_error("Text string is empty.");
  }

  m_theText = str;
}

void TextDraw::TextDrawImpl::SetFontColorFunc(const FontColorFunc& f)
{
  m_getFontColor = f;
}

void TextDraw::TextDrawImpl::SetOutlineFontColorFunc(const FontColorFunc& f)
{
  m_getOutlineFontColor = f;
}

void TextDraw::TextDrawImpl::Prepare()
{
  if (!m_face)
  {
    throw std::logic_error("Font face has not been set.");
  }

  m_textSpans.resize(0);

  int xMax = 0;
  int yMin = 100000;
  int yMax = 0;
  for (size_t i = 0; i < m_theText.size(); i++)
  {
    // Load the glyph we are looking for.
    const FT_UInt gIndex = FT_Get_Char_Index(m_face, static_cast<FT_ULong>(m_theText[i]));
    if (FT_Load_Glyph(m_face, gIndex, FT_LOAD_NO_BITMAP) != 0)
    {
      throw std::runtime_error(std20::format("Could not load font for glyph index {}.", gIndex));
    }

    // Need an outline for this to work.
    if (m_face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
    {
      throw std::logic_error(
          std20::format("Not a correct font format: {}.", m_face->glyph->format));
    }

    const Spans spans = GetSpans(i);
    (void)m_textSpans.emplace_back(spans);

    xMax += spans.advance;
    yMin = std::min(yMin, spans.rect.ymin);
    yMax = std::max(yMax, spans.rect.ymax);
  }

  m_textBoundingRect.xmin = 0;
  m_textBoundingRect.xmax = xMax;
  m_textBoundingRect.ymin = yMin;
  m_textBoundingRect.ymax = yMax;
}

auto TextDraw::TextDrawImpl::GetStartXPen(const int xPen) const -> int
{
  switch (m_textAlignment)
  {
    case TextAlignment::LEFT:
      return xPen;
    case TextAlignment::CENTER:
      return xPen - (GetPreparedTextBoundingRect().xmax - GetPreparedTextBoundingRect().xmin) / 2;
    case TextAlignment::RIGHT:
      return xPen - (GetPreparedTextBoundingRect().xmax - GetPreparedTextBoundingRect().xmin);
    default:
      throw std::logic_error("Unknown TextAlignment value.");
  }
}

auto TextDraw::TextDrawImpl::GetStartYPen(const int yPen) -> int
{
  return yPen;
}

void TextDraw::TextDrawImpl::Draw(int xPen, int yPen, PixelBuffer& buff)
{
  int xNext;
  int yNext;
  Draw(xPen, yPen, xNext, yNext, buff);
}

void TextDraw::TextDrawImpl::Draw(int xPen, int yPen, int& xNext, int& yNext, PixelBuffer& buff)
{
  if (m_textSpans.empty())
  {
    throw std::logic_error("textSpans is empty.");
  }

  xNext = GetStartXPen(xPen);
  yNext = GetStartYPen(yPen);

  for (auto& s : m_textSpans)
  {
    WriteGlyph(s, xNext, m_screenHeight - yNext, buff);
    xNext += s.advance;
  }
}

// A horizontal pixel span generated by the FreeType renderer.
struct TextDraw::TextDrawImpl::Vec2
{
  Vec2(const int a, const int b) : x{a}, y{b} {}
  int x;
  int y;
};

TextDraw::TextDrawImpl::RectImpl::RectImpl(const int left,
                                           const int top,
                                           const int right,
                                           const int bottom) noexcept
{
  xmin = left;
  xmax = right;
  ymin = top;
  ymax = bottom;
}

void TextDraw::TextDrawImpl::RectImpl::Include(const Vec2& span)
{
  xmin = std::min(xmin, span.x);
  ymin = std::min(ymin, span.y);
  xmax = std::max(xmax, span.x);
  ymax = std::max(ymax, span.y);
}

struct TextDraw::TextDrawImpl::Span
{
  Span(const int _x, const int _y, const int _width, const int _coverage)
    : x{_x}, y{_y}, width{_width}, coverage{_coverage}
  {
  }

  int x;
  int y;
  int width;
  int coverage;
};

// Render the specified character as a colored glyph with a colored outline.
void TextDraw::TextDrawImpl::WriteGlyph(const Spans& spans,
                                        const int xPen,
                                        const int yPen,
                                        PixelBuffer& buff) const
{
  // Loop over the outline spans and just draw them into the image.
  WriteSpansToImage(spans.outlineSpans, spans.rect, xPen, yPen, spans.textIndexOfChar,
                    m_getOutlineFontColor, buff);

  // Then loop over the regular glyph spans and blend them into the image.
  WriteSpansToImage(spans.stdSpans, spans.rect, xPen, yPen, spans.textIndexOfChar, m_getFontColor,
                    buff);
}

void TextDraw::TextDrawImpl::WriteSpansToImage(const SpanArray& spans,
                                               const RectImpl& rect,
                                               const int xPen,
                                               const int yPen,
                                               const size_t textIndexOfChar,
                                               const FontColorFunc& getColor,
                                               PixelBuffer& buff) const
{
  for (const auto& s : spans)
  {
    const int yPos = m_screenHeight - (yPen + s.y);
    if (yPos < 0 || yPos >= m_screenHeight)
    {
      continue;
    }

    const int xPos0 = xPen + s.x - rect.xmin;
    const int xf0 = s.x - rect.xmin;
    const auto coverage = static_cast<uint8_t>(s.coverage);
    for (int w = 0; w < s.width; ++w)
    {
      const int xPos = xPos0 + w;
      if (xPos < 0 || xPos >= m_screenWidth)
      {
        continue;
      }

      const Pixel color = getColor(textIndexOfChar, xf0 + w, rect.Height() - (s.y - rect.ymin),
                                   rect.Width(), rect.Height());
      const Pixel srceColor{
          {/*.r = */ color.R(), /*.g = */ color.G(), /*.b = */ color.B(), /*.a = */ coverage}};

      Pixel& destColor = buff(static_cast<size_t>(xPos), static_cast<size_t>(yPos));
      destColor = UTILS::GetColorBlend(srceColor, destColor);
    }
  }
}

// clang compiler doesn't like 'static constexpr'
/*constexpr*/ inline auto TextDraw::TextDrawImpl::ToStdPixelCoord(const int freeTypeCoord) -> int
{
  return freeTypeCoord >> 6;
}

/*constexpr*/ inline auto TextDraw::TextDrawImpl::ToFreeTypeCoord(const int stdPixelCoord) -> int
{
  return stdPixelCoord << 6;
}

/*constexpr*/ inline auto TextDraw::TextDrawImpl::ToFreeTypeCoord(const float stdPixelCoord) -> int
{
  return static_cast<int>(std::lround(stdPixelCoord * 64.0));
}

// Each time the renderer calls us back we just push another span entry on our list.
void TextDraw::TextDrawImpl::RasterCallback(const int y,
                                            const int count,
                                            const FT_Span* const spans,
                                            void* const user)
{
  auto* userSpans = static_cast<SpanArray*>(user);
  for (int i = 0; i < count; ++i)
  {
    userSpans->push_back(Span{spans[i].x, y, spans[i].len, spans[i].coverage});
  }
}

// Set up the raster parameters and render the outline.
void TextDraw::TextDrawImpl::RenderSpans(FT_Outline* const outline, SpanArray* const spans) const
{
  FT_Raster_Params params;
  (void)memset(&params, 0, sizeof(params));
  params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
  params.gray_spans = RasterCallback;
  params.user = spans;

  (void)FT_Outline_Render(m_library, outline, &params);
}

auto TextDraw::TextDrawImpl::GetSpans(const size_t textIndexOfChar) const -> Spans
{
  const SpanArray stdSpans = GetStdSpans();
  const int advance = ToStdPixelCoord(m_face->glyph->advance.x) +
                      static_cast<int>(m_charSpacing * static_cast<float>(m_fontSize));
  FT_Glyph_Metrics metrics = m_face->glyph->metrics;
  if (stdSpans.empty())
  {
    return Spans{
        /*.stdSpans = */ stdSpans,
        /*.outlineSpans = */ SpanArray{},
        /*.textIndexOfChar = */ textIndexOfChar,
        /*.rect = */ RectImpl{},
        /*.advance = */ advance,
        /*.bearingX = */ ToStdPixelCoord(metrics.horiBearingX),
        /*.bearingY = */ ToStdPixelCoord(metrics.horiBearingY),
    };
  }

  const SpanArray outlineSpans = GetOutlineSpans();
  return Spans{
      /*.stdSpans = */ stdSpans,
      /*.outlineSpans = */ outlineSpans,
      /*.textIndexOfChar = */ textIndexOfChar,
      /*.rect = */ GetBoundingRect(stdSpans, outlineSpans),
      /*.advance = */ advance,
      /*.bearingX = */ ToStdPixelCoord(metrics.horiBearingX),
      /*.bearingY = */ ToStdPixelCoord(metrics.horiBearingY),
  };
}

auto TextDraw::TextDrawImpl::GetStdSpans() const -> SpanArray
{
  SpanArray spans{};

  RenderSpans(&m_face->glyph->outline, &spans);

  return spans;
}

auto TextDraw::TextDrawImpl::GetOutlineSpans() const -> SpanArray
{
  // Set up a stroker.
  FT_Stroker stroker{};
  (void)FT_Stroker_New(m_library, &stroker);
  FT_Stroker_Set(stroker, ToFreeTypeCoord(m_outlineWidth), FT_STROKER_LINECAP_ROUND,
                 FT_STROKER_LINEJOIN_ROUND, 0);

  FT_Glyph glyph{};
  if (FT_Get_Glyph(m_face->glyph, &glyph) != 0)
  {
    throw std::runtime_error("Could not get glyph for outline spans.");
  }

  // Next we need the spans for the outline.
  (void)FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);

  // Again, this needs to be an outline to work.
  if (glyph->format != FT_GLYPH_FORMAT_OUTLINE)
  {
    throw std::runtime_error("Glyph does not have outline format.");
  }

  // Render the outline spans to the span list
  FT_Outline* outline = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
  SpanArray outlineSpans{};
  RenderSpans(outline, &outlineSpans);
  if (outlineSpans.empty())
  {
    throw std::logic_error("Rendered outline spans are empty.");
  }

  // Clean up afterwards.
  FT_Stroker_Done(stroker);
  FT_Done_Glyph(glyph);

  return outlineSpans;
}

auto TextDraw::TextDrawImpl::GetPreparedTextBoundingRect() const -> TextDraw::Rect
{
  return m_textBoundingRect;
}

auto TextDraw::TextDrawImpl::GetBearingX() const -> int
{
  return m_textSpans.front().bearingX;
}

auto TextDraw::TextDrawImpl::GetBearingY() const -> int
{
  return m_textSpans.front().bearingY;
}

auto TextDraw::TextDrawImpl::GetBoundingRect(const SpanArray& stdSpans,
                                             const SpanArray& outlineSpans) -> RectImpl
{
  RectImpl rect{stdSpans.front().x, stdSpans.front().y, stdSpans.front().x, stdSpans.front().y};

  for (const auto& s : stdSpans)
  {
    rect.Include(Vec2{s.x, s.y});
    rect.Include(Vec2{s.x + s.width - 1, s.y});
  }
  for (const auto& s : outlineSpans)
  {
    rect.Include(Vec2{s.x, s.y});
    rect.Include(Vec2{s.x + s.width - 1, s.y});
  }

  return rect;
}

/***
void TextDraw::TextDrawImpl::WriteGlyphSimple(
    const wchar_t ch, const int xPen, const int yPen, int& advance, PixelBuffer& buff)
{
  // Set the size to use.
  if (FT_Set_Char_Size(m_face, toFreeTypeCoord(m_fontSize), toFreeTypeCoord(m_fontSize),
                       m_horizontalResolution, m_verticalResolution) != 0)
  {
    throw std::runtime_error(std20::format("Could not set font size to {}.", m_fontSize));
  }

  const float angle = (25.0F / 360.0F) * 3.14159F * 2; // use 25 degrees

  FT_GlyphSlot slot = face->glyph;

  FT_Matrix matrix;
  matrix.xx = (FT_Fixed)(+std::cos(angle) * 0x10000L);
  matrix.xy = (FT_Fixed)(-std::sin(angle) * 0x10000L);
  matrix.yx = (FT_Fixed)(+std::sin(angle) * 0x10000L);
  matrix.yy = (FT_Fixed)(+std::cos(angle) * 0x10000L);

  FT_Vector pen{ToFreeTypeCoord(xPen), ToFreeTypeCoord(yPen)};
  FT_Set_Transform(m_face, &matrix, &pen);

  // load glyph image into the slot (erase previous one)
  if (FT_Load_Char(m_face, static_cast<FT_ULong>(ch), FT_LOAD_RENDER))
  {
    throw std::runtime_error(std20::format("Could not load face for '{}'.", static_cast<char>(ch)));
  }

  // now, draw to our target surface (convert position)
  DrawBitmap(&slot->bitmap, slot->bitmap_left, m_screenHeight - slot->bitmap_top, ch, buff);

  advance = ToStdPixelCoord(slot->advance.x);
}

void TextDraw::TextDrawImpl::DrawBitmap(
    FT_Bitmap* bitmap, const FT_Int x, const FT_Int y, const wchar_t ch, PixelBuffer& buff) const
{
  const auto xMax = static_cast<uint32_t>(x) + bitmap->width;
  const auto yMax = static_cast<uint32_t>(y) + bitmap->rows;

  // for simplicity, we assume that `bitmap->pixel_mode'
  // is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)

  for (int i = x, p = 0; i < static_cast<int>(xMax); i++, p++)
  {
    for (int j = y, q = 0; j < static_cast<int>(yMax); j++, q++)
    {
      if (i < 0 || j < 0 || i >= m_screenWidth || j >= m_screenHeight)
      {
        continue;
      }

      const int color =
          bitmap->buffer[static_cast<size_t>(q * static_cast<int>(bitmap->width) + p)];
      if (color == 0)
      {
        buff(static_cast<size_t>(i), static_cast<size_t>(j)) = Pixel::BLACK;
      }
      else
      {
        buff(static_cast<size_t>(i), static_cast<size_t>(j)) =
            m_getFontColor(ch, p, q, bitmap->width, bitmap->rows);
      }
    }
  }
}
**/

} // namespace GOOM
