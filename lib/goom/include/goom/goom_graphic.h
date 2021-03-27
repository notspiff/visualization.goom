#ifndef VISUALIZATION_GOOM_GOOM_GRAPHIC_H
#define VISUALIZATION_GOOM_GOOM_GRAPHIC_H

#include "goom_config.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <tuple>
#include <vector>

namespace GOOM
{

template<class T>
struct channel_limits
{
  static constexpr auto min() noexcept -> T { return T(); }
  static constexpr auto max() noexcept -> T { return T(); }
};
template<>
struct channel_limits<uint8_t>
{
  static constexpr auto min() noexcept -> uint8_t { return 0; }
  static constexpr auto max() noexcept -> uint8_t { return 255; }
};
template<>
struct channel_limits<uint32_t>
{
  static constexpr auto min() noexcept -> uint32_t { return channel_limits<uint8_t>::min(); }
  static constexpr auto max() noexcept -> uint32_t { return channel_limits<uint8_t>::max(); }
};
template<>
struct channel_limits<int32_t>
{
  static constexpr auto min() noexcept -> int32_t { return channel_limits<uint8_t>::min(); }
  static constexpr auto max() noexcept -> int32_t { return channel_limits<uint8_t>::max(); }
};
template<>
struct channel_limits<float>
{
  static constexpr auto min() noexcept -> float { return channel_limits<uint8_t>::min(); }
  static constexpr auto max() noexcept -> float { return channel_limits<uint8_t>::max(); }
};

constexpr uint8_t MAX_COLOR_VAL = channel_limits<uint8_t>::max();

class Pixel
{
#ifdef COLOR_BGRA
  struct Channels
  {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
  };
#else
  struct Channels
  {
    uint8_t a = 0;
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
  };
#endif /* COLOR_BGRA */

public:
  Pixel();
  explicit Pixel(const Channels& c);
  explicit Pixel(uint32_t val);

  [[nodiscard]] auto R() const -> uint8_t;
  [[nodiscard]] auto G() const -> uint8_t;
  [[nodiscard]] auto B() const -> uint8_t;
  [[nodiscard]] auto A() const -> uint8_t;

  void SetR(uint8_t c);
  void SetG(uint8_t c);
  void SetB(uint8_t c);
  void SetA(uint8_t c);

  [[nodiscard]] auto RFlt() const -> float;
  [[nodiscard]] auto GFlt() const -> float;
  [[nodiscard]] auto BFlt() const -> float;

  [[nodiscard]] auto Rgba() const -> uint32_t;
  void SetRgba(uint32_t v);

  static const Pixel BLACK;
  static const Pixel WHITE;

  auto operator==(const Pixel& p) const -> bool;

private:
  union Color
  {
    Channels channels{};
    uint32_t intVal;
  };
  Color m_color{};
};

#if __cplusplus > 201402L
inline const Pixel Pixel::BLACK{/*.channels*/ {/*.r = */ 0, /*.g = */ 0, /*.b = */ 0,
                                               /*.a = */ MAX_COLOR_VAL}};

inline const Pixel Pixel::WHITE{/*.channels*/ {/*.r = */ MAX_COLOR_VAL, /*.g = */ MAX_COLOR_VAL,
                                               /*.b = */ MAX_COLOR_VAL, /*.a = */ MAX_COLOR_VAL}};
#endif

class PixelBuffer
{
  using Buffer = std::vector<Pixel>;

public:
  PixelBuffer() noexcept = default;
  PixelBuffer(uint32_t width, uint32_t height) noexcept;
  virtual ~PixelBuffer() noexcept = default;
  PixelBuffer(const PixelBuffer&) noexcept = delete;
  PixelBuffer(PixelBuffer&&) noexcept = delete;
  auto operator=(const PixelBuffer&) -> PixelBuffer& = delete;
  auto operator=(PixelBuffer&&) -> PixelBuffer& = delete;

  void Resize(size_t width, size_t height);

  [[nodiscard]] auto GetWidth() const -> uint32_t;
  [[nodiscard]] auto GetHeight() const -> uint32_t;

  void Fill(const Pixel& c);
  void CopyTo(PixelBuffer& buff, uint32_t length) const;
  [[nodiscard]] auto GetIntBuff() const -> const uint32_t*;

  auto operator()(size_t x, size_t y) const -> const Pixel&;
  auto operator()(size_t x, size_t y) -> Pixel&;

  using iterator = Buffer::iterator;
  using const_iterator = Buffer::const_iterator;
  [[nodiscard]] auto GetRowIter(size_t y) -> std::tuple<iterator, iterator>;
  [[nodiscard]] auto GetRowIter(size_t y) const -> std::tuple<const_iterator, const_iterator>;

  [[nodiscard]] auto Get4RHBNeighbours(size_t x, size_t y) const -> std::array<Pixel, 4>;

private:
  uint32_t m_width{};
  uint32_t m_height{};
  Buffer m_buff{};

  [[nodiscard]] auto GetIntBuff() -> uint32_t*;
  void CopyTo(uint32_t* intBuff, uint32_t length) const;
};

inline Pixel::Pixel() : m_color{/*.channels*/ {}}
{
}

inline Pixel::Pixel(const Channels& c) : m_color{/*.channels*/ {c}}
{
}

inline Pixel::Pixel(const uint32_t v)
{
  m_color.intVal = v;
}

inline auto Pixel::operator==(const Pixel& p) const -> bool
{
  return Rgba() == p.Rgba();
}

inline auto Pixel::R() const -> uint8_t
{
  return m_color.channels.r;
}

inline void Pixel::SetR(const uint8_t c)
{
  m_color.channels.r = c;
}

inline auto Pixel::G() const -> uint8_t
{
  return m_color.channels.g;
}

inline void Pixel::SetG(const uint8_t c)
{
  m_color.channels.g = c;
}

inline auto Pixel::B() const -> uint8_t
{
  return m_color.channels.b;
}

inline void Pixel::SetB(const uint8_t c)
{
  m_color.channels.b = c;
}

inline auto Pixel::A() const -> uint8_t
{
  return m_color.channels.a;
}

inline void Pixel::SetA(const uint8_t c)
{
  m_color.channels.a = c;
}

inline auto Pixel::RFlt() const -> float
{
  return static_cast<float>(R()) / channel_limits<float>::max();
}

inline auto Pixel::GFlt() const -> float
{
  return static_cast<float>(G()) / channel_limits<float>::max();
}

inline auto Pixel::BFlt() const -> float
{
  return static_cast<float>(B()) / channel_limits<float>::max();
}

inline auto Pixel::Rgba() const -> uint32_t
{
  return m_color.intVal;
}

inline void Pixel::SetRgba(const uint32_t v)
{
  m_color.intVal = v;
}

inline PixelBuffer::PixelBuffer(const uint32_t w, const uint32_t h) noexcept
  : m_width{w}, m_height{h}, m_buff(m_width * m_height)
{
}

inline void PixelBuffer::Resize(const size_t width, const size_t height)
{
  m_width = width;
  m_height = height;
  m_buff.resize(m_width * m_height);
}

inline auto PixelBuffer::GetWidth() const -> uint32_t
{
  return m_width;
}

inline auto PixelBuffer::GetHeight() const -> uint32_t
{
  return m_height;
}

inline void PixelBuffer::Fill(const Pixel& c)
{
  std::fill(m_buff.begin(), m_buff.end(), c);
}

inline auto PixelBuffer::GetIntBuff() const -> const uint32_t*
{
  return reinterpret_cast<const uint32_t*>(m_buff.data());
}

inline auto PixelBuffer::GetIntBuff() -> uint32_t*
{
  return reinterpret_cast<uint32_t*>(m_buff.data());
}

inline void PixelBuffer::CopyTo(PixelBuffer& buff, const uint32_t length) const
{
  CopyTo(buff.GetIntBuff(), length);
}

inline void PixelBuffer::CopyTo(uint32_t* intBuff, const uint32_t length) const
{
  static_assert(sizeof(Pixel) == sizeof(uint32_t), "Invalid Pixel size.");
  std::memcpy(intBuff, GetIntBuff(), length * sizeof(Pixel));
}

inline auto PixelBuffer::operator()(const size_t x, const size_t y) const -> const Pixel&
{
  //return m_buff.at(y * m_width + x);
  return m_buff[y * m_width + x];
}

inline auto PixelBuffer::operator()(const size_t x, const size_t y) -> Pixel&
{
  //return m_buff.at(y * m_width + x);
  return m_buff[y * m_width + x];
}

inline auto PixelBuffer::GetRowIter(const size_t y)
    -> std::tuple<PixelBuffer::iterator, PixelBuffer::iterator>
{
  const auto rowPos = static_cast<int32_t>(y * m_width);
  return std::make_tuple(m_buff.begin() + rowPos, m_buff.begin() + rowPos + m_width);
}

inline auto PixelBuffer::GetRowIter(const size_t y) const
    -> std::tuple<PixelBuffer::const_iterator, PixelBuffer::const_iterator>
{
  const auto rowPos = static_cast<int32_t>(y * m_width);
  return std::make_tuple(m_buff.begin() + rowPos, m_buff.begin() + rowPos + m_width);
}

inline auto PixelBuffer::Get4RHBNeighbours(const size_t x, const size_t y) const
    -> std::array<Pixel, 4>
{
  assert(x < m_width && y < m_height);
  //if (x >= m_width || y >= m_height)
  //{
  //  return {Pixel::BLACK, Pixel::BLACK, Pixel::BLACK, Pixel::BLACK};
  //}

  const size_t xPos = y * m_width + x;

  if (x >= m_width - 1 && y >= m_height - 1)
  {
    return {m_buff[xPos], Pixel::BLACK, Pixel::BLACK, Pixel::BLACK};
  }

  if (x >= m_width - 1)
  {
    return {m_buff[xPos], Pixel::BLACK, m_buff[xPos + m_width], Pixel::BLACK};
  }

  if (y >= m_height - 1)
  {
    return {m_buff[xPos], m_buff[xPos + 1], Pixel::BLACK, Pixel::BLACK};
  }

  return {
      m_buff[xPos],
      m_buff[xPos + 1],
      m_buff[xPos + m_width],
      m_buff[xPos + m_width + 1],
  };
}

} // namespace GOOM
#endif
