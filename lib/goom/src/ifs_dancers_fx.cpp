/*
 * ifs.c --- modified iterated functions system for goom.
 */

/*-
 * Copyright (c) 1997 by Massimino Pascal <Pascal.Massimon@ens.fr>
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 * If this mode is weird and you have an old MetroX server, it is buggy.
 * There is a free SuSE-enhanced MetroX X server that is fine.
 *
 * When shown ifs, Diana Rose (4 years old) said, "It looks like dancing."
 *
 * Revision History:
 * 01-Feb-2021: converted to C++14, lots of color changes (glk)
 * 13-Dec-2003: Added some goom specific stuffs (to make ifs a VisualFX).
 * 11-Apr-2002: jeko@ios-software.com: Make ifs.c system-indendant. (ifs.h added)
 * 01-Nov-2000: Allocation checks
 * 10-May-1997: jwz@jwz.org: turned into a standalone program.
 *              Made it render into an offscreen bitmap and then copy
 *              that onto the screen, to reduce flicker.
 */

/* #ifdef STANDALONE */
#include "ifs_dancers_fx.h"

#include "goom_config.h"
#include "goom_draw.h"
#include "goom_graphic.h"
#include "goom_plugin_info.h"
#include "goom_stats.h"
#include "goom_visual_fx.h"
#include "goomutils/colormaps.h"
#include "goomutils/colorutils.h"
#include "goomutils/goomrand.h"
#include "goomutils/image_bitmaps.h"
#include "goomutils/logging_control.h"
// #undef NO_LOGGING
#include "goomutils/logging.h"
#include "goomutils/mathutils.h"
#include "goomutils/random_colormaps.h"
#include "goomutils/random_colormaps_manager.h"
#include "goomutils/small_image_bitmaps.h"
#include "stats/ifs_stats.h"

#include <array>

//#undef NDEBUG
#include <cassert>
#include <cmath>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace GOOM
{

using namespace GOOM::UTILS;
using COLOR_DATA::ColorMapName;

struct Similitude;

inline auto MegaChangeColorMapEvent() -> bool
{
  return ProbabilityOfMInN(3, 10);
}

inline auto AllowOverexposedEvent() -> bool
{
  return ProbabilityOfMInN(10, 50);
}

struct IfsPoint
{
  uint32_t x = 0;
  uint32_t y = 0;
  uint32_t count = 0;
  Pixel color{0U};
  const Similitude* simi{};
};

using Dbl = float;
using Flt = int;

struct FltPoint
{
  Flt x = 0;
  Flt y = 0;
};

constexpr int FIX = 12;

inline auto DblToFlt(const Dbl x) -> Flt
{
  constexpr int UNIT = 1 << FIX;
  return static_cast<Flt>(static_cast<Dbl>(UNIT) * x);
}

inline auto DivByUnit(const Flt x) -> Flt
{
  return x >> FIX;
}

inline auto DivBy2Units(const Flt x) -> Flt
{
  return x >> (FIX + 1);
}

struct CentreType
{
  uint32_t depth;
  Dbl r1Mean;
  Dbl r2Mean;
  Dbl dr1Mean;
  Dbl dr2Mean;
};
// clang-format off
static const std::vector<CentreType> CENTRE_LIST = {
  { /*.depth = */10, /*.r1Mean = */0.7, /*.r2Mean = */0.0, /*.dr1Mean = */0.3, /*.dr2Mean = */0.4 },
  { /*.depth = */ 6, /*.r1Mean = */0.6, /*.r2Mean = */0.0, /*.dr1Mean = */0.4, /*.dr2Mean = */0.3 },
  { /*.depth = */ 4, /*.r1Mean = */0.5, /*.r2Mean = */0.0, /*.dr1Mean = */0.4, /*.dr2Mean = */0.3 },
  { /*.depth = */ 2, /*.r1Mean = */0.4, /*.r2Mean = */0.0, /*.dr1Mean = */0.5, /*.dr2Mean = */0.3 },
};
// clang-format on

struct Similitude
{
  Dbl dbl_cx = 0;
  Dbl dbl_cy = 0;
  Dbl dbl_r1 = 0;
  Dbl dbl_r2 = 0;
  Dbl A1 = 0;
  Dbl A2 = 0;
  Flt cosA1 = 0;
  Flt sinA1 = 0;
  Flt cosA2 = 0;
  Flt sinA2 = 0;
  Flt cx = 0;
  Flt cy = 0;
  Flt r1 = 0;
  Flt r2 = 0;
  Pixel color{0U};
  const ImageBitmap* currentPointBitmap{};
};

class FractalHits
{
public:
  FractalHits(uint32_t width, uint32_t height) noexcept;
  void Reset();
  void AddHit(uint32_t x, uint32_t y, const Similitude& s);
  [[nodiscard]] auto GetBuffer() -> const std::vector<IfsPoint>&;
  [[nodiscard]] auto GetMaxHitCount() const -> uint32_t { return m_maxHitCount; }

private:
  const uint32_t m_width = 0;
  const uint32_t m_height = 0;
  struct HitInfo
  {
    uint32_t count = 0;
    Pixel color{0U};
    const Similitude* simi{};
  };
  std::vector<std::vector<HitInfo>> m_hitInfo{};
  uint32_t m_maxHitCount = 0;
  static constexpr size_t HITS_ESTIMATE = 1000;
  std::vector<IfsPoint> m_hits{};
  std::vector<IfsPoint> m_buffer{};
};

FractalHits::FractalHits(const uint32_t w, const uint32_t h) noexcept
  : m_width{w}, m_height{h}, m_hitInfo(m_height)
{
  for (auto& xHit : m_hitInfo)
  {
    xHit.resize(m_width);
  }
  m_hits.reserve(HITS_ESTIMATE);
}

void FractalHits::Reset()
{
  m_maxHitCount = 0;
  m_hits.resize(0);
  for (auto& xHit : m_hitInfo)
  {
    // Optimization makes sense here:
    // std::fill(xHit.begin(), xHit.end(), HitInfo{0, Pixel::BLACK});
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
    (void)std::memset(xHit.data(), 0, xHit.size() * sizeof(HitInfo));
#pragma GCC diagnostic pop
  }
}

void FractalHits::AddHit(const uint32_t x, const uint32_t y, const Similitude& s)
{
  const uint32_t ux = x & 0x7fffffffU;
  const uint32_t uy = y & 0x7fffffffU;

  if (ux >= m_width || uy >= m_height)
  {
    return;
  }

  HitInfo& h = m_hitInfo[uy][ux];

  h.simi = &s;
  h.color = GetColorAverage(h.color, s.color);
  h.count++;

  if (h.count > m_maxHitCount)
  {
    m_maxHitCount = h.count;
  }

  if (h.count == 1)
  {
    (void)m_hits.emplace_back(IfsPoint{ux, uy, 1});
  }
}

auto FractalHits::GetBuffer() -> const std::vector<IfsPoint>&
{
  m_buffer.resize(m_hits.size());
  for (size_t i = 0; i < m_hits.size(); i++)
  {
    IfsPoint pt = m_hits[i];
    pt.count = m_hitInfo[pt.y][pt.x].count;
    pt.color = m_hitInfo[pt.y][pt.x].color;
    pt.simi = m_hitInfo[pt.y][pt.x].simi;
    m_buffer[i] = pt;
  }
  return m_buffer;
}

class Fractal
{
public:
  Fractal(const std::shared_ptr<const PluginInfo>& goomInfo,
          const RandomColorMaps& cm,
          IfsStats* s);
  ~Fractal() noexcept = default;
  Fractal(const Fractal&) noexcept = delete;
  Fractal(const Fractal&&) noexcept = delete;
  auto operator=(const Fractal&) -> Fractal& = delete;
  auto operator=(const Fractal&&) -> Fractal& = delete;

  void Init();
  void ResetCurrentIfsFunc();
  void SetSmallImageBitmaps(const SmallImageBitmaps& smallBitmaps);

  [[nodiscard]] auto GetSpeed() const -> uint32_t { return m_speed; }
  void SetSpeed(const uint32_t val) { m_speed = val; }

  [[nodiscard]] auto GetMaxHitCount() const -> uint32_t { return m_curHits->GetMaxHitCount(); }

  [[nodiscard]] auto DrawIfs() -> const std::vector<IfsPoint>&;

private:
  static constexpr size_t MAX_SIMI = 6;
  static constexpr size_t NUM_SIMI_GROUPS = 5;
  static constexpr size_t MAX_COUNT_TIMES_SPEED = 1000;

  const RandomColorMaps* m_colorMaps{};
  const SmallImageBitmaps* m_smallBitmaps{};
  IfsStats* m_stats{};

  const uint32_t m_lx;
  const uint32_t m_ly;
  uint32_t m_numSimi = 0;
  uint32_t m_depth = 0;
  uint32_t m_count = 0;
  static constexpr uint32_t INITIAL_SPEED = 6;
  uint32_t m_speed = INITIAL_SPEED;
  Dbl m_r1Mean = 0;
  Dbl m_r2Mean = 0;
  Dbl m_dr1Mean = 0;
  Dbl m_dr2Mean = 0;

  std::vector<Similitude> m_components{};

  FractalHits m_hits1;
  FractalHits m_hits2;
  FractalHits* m_prevHits = nullptr;
  FractalHits* m_curHits = nullptr;

  [[nodiscard]] auto GetLx() const -> Flt { return static_cast<Flt>(m_lx); }
  [[nodiscard]] auto GetLy() const -> Flt { return static_cast<Flt>(m_ly); }
  void DrawFractal();
  void RandomSimis(size_t start, size_t num);
  void Trace(uint32_t curDepth, const FltPoint& p0);
  void UpdateHits(const Similitude& simi, const FltPoint& p);
  using IfsFunc =
      std::function<FltPoint(const Similitude& simi, float x1, float y1, float x2, float y2)>;
  IfsFunc m_curFunc{};
  auto Transform(const Similitude& simi, const FltPoint& p0) const -> FltPoint;
  static auto GaussRand(Dbl c, Dbl S, Dbl A_mult_1_minus_exp_neg_S) -> Dbl;
  static auto HalfGaussRand(Dbl c, Dbl S, Dbl A_mult_1_minus_exp_neg_S) -> Dbl;
  static constexpr auto Get_1_minus_exp_neg_S(Dbl S) -> Dbl;
};

Fractal::Fractal(const std::shared_ptr<const PluginInfo>& goomInfo,
                 const RandomColorMaps& cm,
                 IfsStats* s)
  : m_colorMaps{&cm},
    m_stats{s},
    m_lx{(goomInfo->GetScreenInfo().width - 1) / 2},
    m_ly{(goomInfo->GetScreenInfo().height - 1) / 2},
    m_components(NUM_SIMI_GROUPS * MAX_SIMI),
    m_hits1{goomInfo->GetScreenInfo().width, goomInfo->GetScreenInfo().height},
    m_hits2{goomInfo->GetScreenInfo().width, goomInfo->GetScreenInfo().height},
    m_prevHits{&m_hits1},
    m_curHits{&m_hits2}
{
  Init();
  ResetCurrentIfsFunc();
}

void Fractal::Init()
{
  m_prevHits->Reset();
  m_curHits->Reset();

  // clang-format off
  static const Weights<size_t> s_centreWeights{{
    {0, 10},
    {1,  5},
    {2,  3},
    {3,  1},
  }};
  // clang-format on
  assert(s_centreWeights.GetNumElements() == CENTRE_LIST.size());

  const size_t numCentres = 2 + s_centreWeights.GetRandomWeighted();

  m_depth = CENTRE_LIST.at(numCentres - 2).depth;
  m_r1Mean = CENTRE_LIST[numCentres - 2].r1Mean;
  m_r2Mean = CENTRE_LIST[numCentres - 2].r2Mean;
  m_dr1Mean = CENTRE_LIST[numCentres - 2].dr1Mean;
  m_dr2Mean = CENTRE_LIST[numCentres - 2].dr2Mean;

  m_numSimi = numCentres;

  for (size_t i = 0; i < NUM_SIMI_GROUPS; i++)
  {
    RandomSimis(i * MAX_SIMI, MAX_SIMI);
  }
}

inline void Fractal::SetSmallImageBitmaps(const SmallImageBitmaps& smallBitmaps)
{
  m_smallBitmaps = &smallBitmaps;
}

void Fractal::ResetCurrentIfsFunc()
{
  if (ProbabilityOfMInN(3, 10))
  {
    m_curFunc = [&](const Similitude& simi, const float x1, const float y1, const float x2,
                    const float y2) -> FltPoint {
      return {
          DivByUnit(x1 * simi.sinA1 - y1 * simi.cosA1 + x2 * simi.sinA2 - y2 * simi.cosA2) +
              simi.cx,
          DivByUnit(x1 * simi.cosA1 + y1 * simi.sinA1 + x2 * simi.cosA2 + y2 * simi.sinA2) +
              simi.cy,
      };
    };
    m_stats->UpdateReverseIfsFunc();
  }
  else
  {
    m_curFunc = [&](const Similitude& simi, const float x1, const float y1, const float x2,
                    const float y2) -> FltPoint {
      return {
          DivByUnit(x1 * simi.cosA1 - y1 * simi.sinA1 + x2 * simi.cosA2 - y2 * simi.sinA2) +
              simi.cx,
          DivByUnit(x1 * simi.sinA1 + y1 * simi.cosA1 + x2 * simi.sinA2 + y2 * simi.cosA2) +
              simi.cy,
      };
    };
    m_stats->UpdateStdIfsFunc();
  }
}

auto Fractal::DrawIfs() -> const std::vector<IfsPoint>&
{
  const Dbl u = static_cast<Dbl>(m_count * m_speed) / static_cast<Dbl>(MAX_COUNT_TIMES_SPEED);
  const Dbl uSq = u * u;
  const Dbl v = 1.0F - u;
  const Dbl vSq = v * v;
  const Dbl u0 = vSq * v;
  const Dbl u1 = 3.0F * vSq * u;
  const Dbl u2 = 3.0F * v * uSq;
  const Dbl u3 = u * uSq;

  Similitude* s = m_components.data();
  Similitude* s0 = s + m_numSimi;
  Similitude* s1 = s0 + m_numSimi;
  Similitude* s2 = s1 + m_numSimi;
  Similitude* s3 = s2 + m_numSimi;

  for (size_t i = 0; i < m_numSimi; i++)
  {
    s[i].dbl_cx = u0 * s0[i].dbl_cx + u1 * s1[i].dbl_cx + u2 * s2[i].dbl_cx + u3 * s3[i].dbl_cx;
    s[i].dbl_cy = u0 * s0[i].dbl_cy + u1 * s1[i].dbl_cy + u2 * s2[i].dbl_cy + u3 * s3[i].dbl_cy;

    s[i].dbl_r1 = u0 * s0[i].dbl_r1 + u1 * s1[i].dbl_r1 + u2 * s2[i].dbl_r1 + u3 * s3[i].dbl_r1;
    s[i].dbl_r2 = u0 * s0[i].dbl_r2 + u1 * s1[i].dbl_r2 + u2 * s2[i].dbl_r2 + u3 * s3[i].dbl_r2;

    s[i].A1 = u0 * s0[i].A1 + u1 * s1[i].A1 + u2 * s2[i].A1 + u3 * s3[i].A1;
    s[i].A2 = u0 * s0[i].A2 + u1 * s1[i].A2 + u2 * s2[i].A2 + u3 * s3[i].A2;
  }

  m_curHits->Reset();
  DrawFractal();
  const std::vector<IfsPoint>& curBuffer = m_curHits->GetBuffer();
  std::swap(m_prevHits, m_curHits);

  if (m_count < MAX_COUNT_TIMES_SPEED / m_speed)
  {
    m_count++;
  }
  else
  {
    s = m_components.data();
    s0 = s + m_numSimi;
    s1 = s0 + m_numSimi;
    s2 = s1 + m_numSimi;
    s3 = s2 + m_numSimi;

    for (size_t i = 0; i < m_numSimi; i++)
    {
      s1[i].dbl_cx = (2.0 * s3[i].dbl_cx) - s2[i].dbl_cx;
      s1[i].dbl_cy = (2.0 * s3[i].dbl_cy) - s2[i].dbl_cy;

      s1[i].dbl_r1 = (2.0 * s3[i].dbl_r1) - s2[i].dbl_r1;
      s1[i].dbl_r2 = (2.0 * s3[i].dbl_r2) - s2[i].dbl_r2;

      s1[i].A1 = (2.0 * s3[i].A1) - s2[i].A1;
      s1[i].A2 = (2.0 * s3[i].A2) - s2[i].A2;

      s0[i] = s3[i];
    }

    RandomSimis(3 * m_numSimi, m_numSimi);
    RandomSimis(4 * m_numSimi, m_numSimi);

    m_count = 0;
  }

  return curBuffer;
}

void Fractal::DrawFractal()
{
  for (size_t i = 0; i < m_numSimi; i++)
  {
    Similitude& simi = m_components[i];

    simi.cx = DblToFlt(simi.dbl_cx);
    simi.cy = DblToFlt(simi.dbl_cy);

    simi.cosA1 = DblToFlt(std::cos(simi.A1));
    simi.sinA1 = DblToFlt(std::sin(simi.A1));
    simi.cosA2 = DblToFlt(std::cos(simi.A2));
    simi.sinA2 = DblToFlt(std::sin(simi.A2));

    simi.r1 = DblToFlt(simi.dbl_r1);
    simi.r2 = DblToFlt(simi.dbl_r2);
  }

  for (size_t i = 0; i < m_numSimi; i++)
  {
    const FltPoint p0{m_components[i].cx, m_components[i].cy};

    for (size_t j = 0; j < m_numSimi; j++)
    {
      if (i != j)
      {
        const FltPoint p = Transform(m_components[j], p0);
        Trace(m_depth, p);
      }
    }
  }
}

constexpr auto Fractal::Get_1_minus_exp_neg_S(const Dbl S) -> Dbl
{
  return 1.0F - std::exp(-S);
}

auto Fractal::GaussRand(const Dbl c, const Dbl S, const Dbl A_mult_1_minus_exp_neg_S) -> Dbl
{
  const Dbl x = GetRandInRange(0.0F, 1.0F);
  const Dbl y = A_mult_1_minus_exp_neg_S * (1.0F - std::exp(-x * x * S));
  return ProbabilityOfMInN(1, 2) ? c + y : c - y;
}

auto Fractal::HalfGaussRand(const Dbl c, const Dbl S, const Dbl A_mult_1_minus_exp_neg_S) -> Dbl
{
  const Dbl x = GetRandInRange(0.0F, 1.0F);
  const Dbl y = A_mult_1_minus_exp_neg_S * (1.0F - std::exp(-x * x * S));
  return c + y;
}

void Fractal::RandomSimis(const size_t start, const size_t num)
{
#if __cplusplus <= 201402L
  static const Dbl c_factor = 0.8F * Get_1_minus_exp_neg_S(4.0);
  static const Dbl r1_1_minus_exp_neg_S = Get_1_minus_exp_neg_S(3.0);
  static const Dbl r2_1_minus_exp_neg_S = Get_1_minus_exp_neg_S(2.0);
  static const Dbl A1_factor = 360.0F * Get_1_minus_exp_neg_S(4.0);
  static const Dbl A2_factor = A1_factor;
#else
  static const constinit Dbl c_factor = 0.8f * Get_1_minus_exp_neg_S(4.0);
  static const constinit Dbl r1_1_minus_exp_neg_S = Get_1_minus_exp_neg_S(3.0);
  static const constinit Dbl r2_1_minus_exp_neg_S = Get_1_minus_exp_neg_S(2.0);
  static const constinit Dbl A1_factor = 360.0F * Get_1_minus_exp_neg_S(4.0);
  static const constinit Dbl A2_factor = A1_factor;
#endif

  const Dbl r1Factor = m_dr1Mean * r1_1_minus_exp_neg_S;
  const Dbl r2Factor = m_dr2Mean * r2_1_minus_exp_neg_S;

  const ColorMapGroup colorMapGroup = m_colorMaps->GetRandomGroup();

  for (size_t i = start; i < start + num; i++)
  {
    m_components[i].dbl_cx = GaussRand(0.0, 4.0, c_factor);
    m_components[i].dbl_cy = GaussRand(0.0, 4.0, c_factor);
    m_components[i].dbl_r1 = GaussRand(m_r1Mean, 3.0, r1Factor);
    m_components[i].dbl_r2 = HalfGaussRand(m_r2Mean, 2.0, r2Factor);
    m_components[i].A1 = GaussRand(0.0, 4.0, A1_factor) * (m_pi / 180.0);
    m_components[i].A2 = GaussRand(0.0, 4.0, A2_factor) * (m_pi / 180.0);
    m_components[i].cosA1 = 0;
    m_components[i].sinA1 = 0;
    m_components[i].cosA2 = 0;
    m_components[i].sinA2 = 0;
    m_components[i].cx = 0;
    m_components[i].cy = 0;
    m_components[i].r1 = 0;
    m_components[i].r2 = 0;

    m_components[i].color =
        RandomColorMaps::GetRandomColor(m_colorMaps->GetRandomColorMap(colorMapGroup), 0.0F, 1.0F);

    if (m_smallBitmaps == nullptr || ProbabilityOfMInN(7, 10))
    {
      m_components[i].currentPointBitmap = nullptr;
    }
    else
    {
      constexpr uint32_t MIN_RES = 3;
      constexpr uint32_t MAX_RES = 5;
      uint32_t res = GetRandInRange(MIN_RES, MAX_RES);
      if (res % 2 == 0)
      {
        res += 1;
      }
      if (ProbabilityOfMInN(1, 2))
      {
        m_components[i].currentPointBitmap =
            &m_smallBitmaps->GetImageBitmap(SmallImageBitmaps::ImageNames::SPHERE, res);
      }
      else
      {
        m_components[i].currentPointBitmap =
            &m_smallBitmaps->GetImageBitmap(SmallImageBitmaps::ImageNames::CIRCLE, res);
      }
    }
  }
}

void Fractal::Trace(const uint32_t curDepth, const FltPoint& p0)
{
  for (size_t i = 0; i < m_numSimi; i++)
  {
    const FltPoint p = Transform(m_components[i], p0);

    UpdateHits(m_components[i], p);

    if (!curDepth)
    {
      continue;
    }
    if (((p.x - p0.x) >> 4) == 0 || ((p.y - p0.y) >> 4) == 0)
    {
      continue;
    }

    Trace(curDepth - 1, p);
  }
}

inline auto Fractal::Transform(const Similitude& simi, const FltPoint& p0) const -> FltPoint
{
  const Flt x1 = DivByUnit((p0.x - simi.cx) * simi.r1);
  const Flt y1 = DivByUnit((p0.y - simi.cy) * simi.r1);

  const Flt x2 = DivByUnit((+x1 - simi.cx) * simi.r2);
  const Flt y2 = DivByUnit((-y1 - simi.cy) * simi.r2);

  return m_curFunc(simi, static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2),
                   static_cast<float>(y2));
}

inline void Fractal::UpdateHits(const Similitude& simi, const FltPoint& p)
{
  const auto x = static_cast<uint32_t>(GetLx() + DivBy2Units(p.x * GetLx()));
  const auto y = static_cast<uint32_t>(GetLy() - DivBy2Units(p.y * GetLy()));
  m_curHits->AddHit(x, y, simi);
}

class Colorizer
{
public:
  Colorizer() noexcept;
  ~Colorizer() noexcept = default;
  Colorizer(const Colorizer&) = delete;
  Colorizer(const Colorizer&&) = delete;
  auto operator=(const Colorizer&) -> Colorizer& = delete;
  auto operator=(const Colorizer&&) -> Colorizer& = delete;

  auto GetColorMaps() const -> const RandomColorMaps&;

  auto GetColorMode() const -> IfsDancersFx::ColorMode;
  void SetForcedColorMode(IfsDancersFx::ColorMode c);
  void ChangeColorMode();

  void ChangeColorMaps();

  void SetMaxHitCount(uint32_t val);

  [[nodiscard]] auto GetMixedColor(
      const Pixel& baseColor, uint32_t hitCount, float tMix, float x, float y) -> Pixel;

private:
  std::shared_ptr<WeightedColorMaps> m_colorMaps{
      std::make_shared<WeightedColorMaps>(Weights<ColorMapGroup>{{
          {ColorMapGroup::PERCEPTUALLY_UNIFORM_SEQUENTIAL, 10},
          {ColorMapGroup::SEQUENTIAL, 10},
          {ColorMapGroup::SEQUENTIAL2, 10},
          {ColorMapGroup::CYCLIC, 10},
          {ColorMapGroup::DIVERGING, 20},
          {ColorMapGroup::DIVERGING_BLACK, 1},
          {ColorMapGroup::QUALITATIVE, 10},
          {ColorMapGroup::MISC, 20},
      }})};
  RandomColorMapsManager m_colorMapsManager{};
  uint32_t m_mixerMap1Id{};
  std::shared_ptr<const IColorMap> m_prevMixerMap1{};
  uint32_t m_mixerMap2Id{};
  std::shared_ptr<const IColorMap> m_prevMixerMap2{};
  mutable uint32_t m_countSinceColorMapChange = 0;
  static constexpr uint32_t MIN_COLOR_MAP_CHANGE_COMPLETED = 5;
  static constexpr uint32_t MAX_COLOR_MAP_CHANGE_COMPLETED = 50;
  uint32_t m_colorMapChangeCompleted = MIN_COLOR_MAP_CHANGE_COMPLETED;

  IfsDancersFx::ColorMode m_colorMode = IfsDancersFx::ColorMode::MAP_COLORS;
  IfsDancersFx::ColorMode m_forcedColorMode = IfsDancersFx::ColorMode::_NULL;
  uint32_t m_maxHitCount = 0;
  float m_logMaxHitCount = 0;
  static constexpr float MIN_T_BETWEEN_COLORS = 0.2;
  static constexpr float MAX_T_BETWEEN_COLORS = 0.8;
  static constexpr float INITIAL_T_BETWEEN_COLORS = 0.5;
  float m_tBetweenColors = INITIAL_T_BETWEEN_COLORS; // in [0, 1]
  static auto GetNextColorMode() -> IfsDancersFx::ColorMode;
  [[nodiscard]] auto GetNextMixerMapColor(float t, float x, float y) const -> Pixel;
};

Colorizer::Colorizer() noexcept
  : m_mixerMap1Id{m_colorMapsManager.AddColorMapInfo(
        {m_colorMaps, ColorMapName::_NULL, RandomColorMaps::EMPTY})},
    m_prevMixerMap1{m_colorMapsManager.GetColorMapPtr(m_mixerMap1Id)},
    m_mixerMap2Id{m_colorMapsManager.AddColorMapInfo(
        {m_colorMaps, ColorMapName::_NULL, RandomColorMaps::EMPTY})},
    m_prevMixerMap2{m_colorMapsManager.GetColorMapPtr(m_mixerMap2Id)}
{
}

inline void Colorizer::SetMaxHitCount(const uint32_t val)
{
  m_maxHitCount = val;
  m_logMaxHitCount = std::log(static_cast<float>(m_maxHitCount));
}

inline auto Colorizer::GetColorMaps() const -> const RandomColorMaps&
{
  return *m_colorMaps;
}

inline auto Colorizer::GetColorMode() const -> IfsDancersFx::ColorMode
{
  return m_colorMode;
}

inline void Colorizer::SetForcedColorMode(const IfsDancersFx::ColorMode c)
{
  m_forcedColorMode = c;
}

void Colorizer::ChangeColorMode()
{
  if (m_forcedColorMode != IfsDancersFx::ColorMode::_NULL)
  {
    m_colorMode = m_forcedColorMode;
  }
  else
  {
    m_colorMode = GetNextColorMode();
  }
}

auto Colorizer::GetNextColorMode() -> IfsDancersFx::ColorMode
{
  // clang-format off
  static const Weights<IfsDancersFx::ColorMode> s_colorModeWeights{{
    { IfsDancersFx::ColorMode::MAP_COLORS,            15 },
    { IfsDancersFx::ColorMode::MEGA_MAP_COLOR_CHANGE, 20 },
    { IfsDancersFx::ColorMode::MIX_COLORS,            10 },
    { IfsDancersFx::ColorMode::MEGA_MIX_COLOR_CHANGE, 15 },
    { IfsDancersFx::ColorMode::REVERSE_MIX_COLORS,     5 },
    { IfsDancersFx::ColorMode::SINGLE_COLORS,          5 },
    { IfsDancersFx::ColorMode::SINE_MIX_COLORS,       15 },
    { IfsDancersFx::ColorMode::SINE_MAP_COLORS,       15 },
  }};
  // clang-format on

  return s_colorModeWeights.GetRandomWeighted();
}

void Colorizer::ChangeColorMaps()
{
  m_prevMixerMap1 = m_colorMapsManager.GetColorMapPtr(m_mixerMap1Id);
  m_prevMixerMap2 = m_colorMapsManager.GetColorMapPtr(m_mixerMap2Id);
  m_colorMapsManager.ChangeAllColorMapsNow();

  //  logInfo("prevMixerMap = {}", enumToString(prevMixerMap->GetMapName()));
  //  logInfo("mixerMap = {}", enumToString(mixerMap->GetMapName()));
  m_colorMapChangeCompleted =
      GetRandInRange(MIN_COLOR_MAP_CHANGE_COMPLETED, MAX_COLOR_MAP_CHANGE_COMPLETED);
  m_tBetweenColors = GetRandInRange(MIN_T_BETWEEN_COLORS, MAX_T_BETWEEN_COLORS);
  m_countSinceColorMapChange = m_colorMapChangeCompleted;
}

inline auto Colorizer::GetNextMixerMapColor(const float t, const float x, const float y) const
    -> Pixel
{
  //  const float angle = y == 0.0F ? m_half_pi : std::atan2(y, x);
  //  const Pixel nextColor = mixerMap1->GetColor((m_pi + angle) / m_two_pi);
  const Pixel nextColor =
      IColorMap::GetColorMix(m_colorMapsManager.GetColorMap(m_mixerMap1Id).GetColor(x),
                             m_colorMapsManager.GetColorMap(m_mixerMap2Id).GetColor(y), t);
  if (m_countSinceColorMapChange == 0)
  {
    return nextColor;
  }

  const float tTransition = static_cast<float>(m_countSinceColorMapChange) /
                            static_cast<float>(m_colorMapChangeCompleted);
  m_countSinceColorMapChange--;
  const Pixel prevNextColor =
      IColorMap::GetColorMix(m_prevMixerMap1->GetColor(x), m_prevMixerMap2->GetColor(y), t);
  return IColorMap::GetColorMix(nextColor, prevNextColor, tTransition);
}

inline auto Colorizer::GetMixedColor(const Pixel& baseColor,
                                     const uint32_t hitCount,
                                     const float tMix,
                                     const float x,
                                     const float y) -> Pixel
{
  constexpr float BRIGHTNESS = 1.9;
  const float logAlpha =
      m_maxHitCount <= 1 ? 1.0F
                         : BRIGHTNESS * std::log(static_cast<float>(hitCount)) / m_logMaxHitCount;

  Pixel mixColor;
  float tBaseMix = 1.0F - m_tBetweenColors;

  switch (m_colorMode)
  {
    case IfsDancersFx::ColorMode::MAP_COLORS:
    case IfsDancersFx::ColorMode::MEGA_MAP_COLOR_CHANGE:
    {
      mixColor = GetNextMixerMapColor(logAlpha, x, y);
      constexpr float MIN_T_BASE_MIX = 0.1;
      constexpr float MAX_T_BASE_MIX = 0.3;
      tBaseMix = GetRandInRange(MIN_T_BASE_MIX, MAX_T_BASE_MIX);
      break;
    }

    case IfsDancersFx::ColorMode::MIX_COLORS:
    case IfsDancersFx::ColorMode::REVERSE_MIX_COLORS:
    case IfsDancersFx::ColorMode::MEGA_MIX_COLOR_CHANGE:
    {
      mixColor = GetNextMixerMapColor(tMix, x, y);
      break;
    }

    case IfsDancersFx::ColorMode::SINGLE_COLORS:
    {
      mixColor = baseColor;
      break;
    }

    case IfsDancersFx::ColorMode::SINE_MIX_COLORS:
    case IfsDancersFx::ColorMode::SINE_MAP_COLORS:
    {
      constexpr float INITIAL_FREQ = 20.0;
      constexpr float T_MIX_FACTOR = 0.5;
      constexpr float Z_STEP = 0.1;
      static float s_freq = INITIAL_FREQ;
      static float s_z = 0.0;

      mixColor = GetNextMixerMapColor(T_MIX_FACTOR * (1.0F + std::sin(s_freq * s_z)), x, y);
      s_z += Z_STEP;
      if (m_colorMode == IfsDancersFx::ColorMode::SINE_MAP_COLORS)
      {
        tBaseMix = 1.0F - m_tBetweenColors;
      }
      break;
    }

    default:
      throw std::logic_error("Unknown ColorMode");
  }

  if (m_colorMode == IfsDancersFx::ColorMode::REVERSE_MIX_COLORS)
  {
    mixColor = IColorMap::GetColorMix(mixColor, baseColor, tBaseMix);
  }
  else
  {
    mixColor = IColorMap::GetColorMix(baseColor, mixColor, tBaseMix);
  }

  constexpr float IFS_GAMMA = 4.2;
  constexpr float IFS_GAMMA_THRESHOLD = 0.01;
  static GammaCorrection s_gammaCorrect{IFS_GAMMA, IFS_GAMMA_THRESHOLD};

  return s_gammaCorrect.GetCorrection(logAlpha, mixColor);
}

class LowDensityBlurrer
{
public:
  LowDensityBlurrer() noexcept = default;
  LowDensityBlurrer(uint32_t screenWidth, uint32_t screenHeight, uint32_t width) noexcept;

  [[nodiscard]] auto GetWidth() const -> uint32_t { return m_width; }
  void SetWidth(uint32_t val);

  void DoBlur(std::vector<IfsPoint>& lowDensityPoints, std::vector<PixelBuffer*>& buffs) const;

private:
  const uint32_t m_screenWidth{};
  const uint32_t m_screenHeight{};
  uint32_t m_width{};
};

inline LowDensityBlurrer::LowDensityBlurrer(const uint32_t screenWidth,
                                            const uint32_t screenHeight,
                                            const uint32_t width) noexcept
  : m_screenWidth{screenWidth}, m_screenHeight{screenHeight}, m_width{width}
{
}

void LowDensityBlurrer::SetWidth(const uint32_t val)
{
  if (val != 3 && val != 5 && val != 7)
  {
    throw std::logic_error(std20::format("Invalid blur width {}.", val));
  }
  m_width = val;
}

void LowDensityBlurrer::DoBlur(std::vector<IfsPoint>& lowDensityPoints,
                               std::vector<PixelBuffer*>& buffs) const
{
  std::vector<Pixel> neighbours(m_width * m_width);

  for (auto& p : lowDensityPoints)
  {
    if (p.x < (m_width / 2) || p.y < (m_width / 2) || p.x >= m_screenWidth - (m_width / 2) ||
        p.y >= m_screenHeight - (m_width / 2))
    {
      p.count = 0; // just signal that no need to set buff
      continue;
    }

    size_t n = 0;
    uint32_t neighY = p.y - m_width / 2;
    for (size_t i = 0; i < m_width; i++)
    {
      uint32_t neighX = p.x - m_width / 2;
      for (size_t j = 0; j < m_width; j++)
      {
        neighbours[n] = (*buffs[0])(neighX, neighY);
        n++;
        neighX++;
      }
      neighY++;
    }
    p.color = GetColorAverage(neighbours);
  }

  for (const auto& p : lowDensityPoints)
  {
    if (p.count == 0)
    {
      continue;
    }
    for (auto& b : buffs)
    {
      (*b)(p.x, p.y) = p.color;
    }
  }
}

class IfsDancersFx::IfsDancersFxImpl
{
public:
  explicit IfsDancersFxImpl(std::shared_ptr<const PluginInfo> info) noexcept;
  ~IfsDancersFxImpl() noexcept;
  IfsDancersFxImpl(const IfsDancersFxImpl&) = delete;
  IfsDancersFxImpl(const IfsDancersFxImpl&&) = delete;
  auto operator=(const IfsDancersFxImpl&) -> IfsDancersFxImpl& = delete;
  auto operator=(const IfsDancersFxImpl&&) -> IfsDancersFxImpl& = delete;

  [[nodiscard]] auto GetResourcesDirectory() const -> const std::string&;
  void SetResourcesDirectory(const std::string& dirName);
  void SetBuffSettings(const FXBuffSettings& settings);
  void SetSmallImageBitmaps(const SmallImageBitmaps& smallBitmaps);

  void Init();

  void ApplyNoDraw();
  void UpdateIfs(PixelBuffer& currentBuff, PixelBuffer& nextBuff);
  void UpdateLowDensityThreshold();
  auto GetColorMode() const -> IfsDancersFx::ColorMode;
  void SetColorMode(IfsDancersFx::ColorMode c);
  void Renew();
  void UpdateIncr();

  void Finish();
  void Log(const GoomStats::LogStatsValueFunc& l) const;

private:
  static constexpr int MAX_COUNT_BEFORE_NEXT_UPDATE = 1000;
  static constexpr int CYCLE_LENGTH = 500;

  const std::shared_ptr<const PluginInfo> m_goomInfo;

  GoomDraw m_draw;
  Colorizer m_colorizer{};
  FXBuffSettings m_buffSettings{};
  std::string m_resourcesDirectory{};

  bool m_allowOverexposed = true;
  uint32_t m_countSinceOverexposed = 0;
  static constexpr uint32_t MAX_COUNT_SINCE_OVEREXPOSED = 100;
  void UpdateAllowOverexposed();

  std::unique_ptr<Fractal> m_fractal{};

  int m_cycle = 0;
  int m_ifsIncr = 1; // dessiner l'ifs (0 = non: > = increment)
  int m_decayIfs = 0; // disparition de l'ifs
  int m_recayIfs = 0; // dedisparition de l'ifs
  void UpdateDecay();
  void UpdateDecayAndRecay();

  IfsStats m_stats{};

  void ChangeColormaps();
  static constexpr uint32_t MAX_DENSITY_COUNT = 20;
  static constexpr uint32_t MIN_DENSITY_COUNT = 5;
  uint32_t m_lowDensityCount = 10;
  LowDensityBlurrer m_blurrer{};
  float m_lowDensityBlurThreshold = 0.40;
  bool m_useRandomLowDensityColors = false;
  auto BlurLowDensityColors(size_t numPoints, const std::vector<IfsPoint>& lowDensityPoints) const
      -> bool;
  void UpdatePixelBuffers(PixelBuffer& currentBuff,
                          PixelBuffer& nextBuff,
                          const std::vector<IfsPoint>& points,
                          uint32_t maxHitCount);
  void DrawPixel(PixelBuffer& currentBuff, PixelBuffer& nextBuff, const IfsPoint& p, float tMix);
  void SetLowDensityColors(std::vector<IfsPoint>& points,
                           uint32_t maxLowDensityCount,
                           std::vector<PixelBuffer*>& buffs) const;
  [[nodiscard]] auto GetIfsIncr() const -> int;
};

IfsDancersFx::IfsDancersFx(const std::shared_ptr<const PluginInfo>& info) noexcept
  : m_fxImpl{new IfsDancersFxImpl{info}}
{
}

IfsDancersFx::~IfsDancersFx() noexcept = default;

void IfsDancersFx::Init()
{
  m_fxImpl->Init();
}

auto IfsDancersFx::GetResourcesDirectory() const -> const std::string&
{
  return m_fxImpl->GetResourcesDirectory();
}

void IfsDancersFx::SetResourcesDirectory(const std::string& dirName)
{
  m_fxImpl->SetResourcesDirectory(dirName);
}

void IfsDancersFx::SetBuffSettings(const FXBuffSettings& settings)
{
  m_fxImpl->SetBuffSettings(settings);
}

void IfsDancersFx::SetSmallImageBitmaps(const SmallImageBitmaps& smallBitmaps)
{
  m_fxImpl->SetSmallImageBitmaps(smallBitmaps);
}

void IfsDancersFx::Start()
{
}

void IfsDancersFx::Finish()
{
  m_fxImpl->Finish();
}

void IfsDancersFx::Log(const GoomStats::LogStatsValueFunc& logVal) const
{
  m_fxImpl->Log(logVal);
}

auto IfsDancersFx::GetFxName() const -> std::string
{
  return "IFS FX";
}

void IfsDancersFx::ApplyNoDraw()
{
  if (!m_enabled)
  {
    return;
  }

  m_fxImpl->ApplyNoDraw();
}

void IfsDancersFx::Apply(PixelBuffer& currentBuff, PixelBuffer& nextBuff)
{
  if (!m_enabled)
  {
    return;
  }

  m_fxImpl->UpdateIfs(currentBuff, nextBuff);
}

auto IfsDancersFx::GetColorMode() const -> IfsDancersFx::ColorMode
{
  return m_fxImpl->GetColorMode();
}

void IfsDancersFx::SetColorMode(ColorMode c)
{
  m_fxImpl->SetColorMode(c);
}

void IfsDancersFx::UpdateIncr()
{
  m_fxImpl->UpdateIncr();
}

void IfsDancersFx::Renew()
{
  m_fxImpl->Renew();
}

IfsDancersFx::IfsDancersFxImpl::IfsDancersFxImpl(std::shared_ptr<const PluginInfo> info) noexcept
  : m_goomInfo{std::move(info)},
    m_draw{m_goomInfo->GetScreenInfo().width, m_goomInfo->GetScreenInfo().height},
    m_fractal{std::make_unique<Fractal>(m_goomInfo, m_colorizer.GetColorMaps(), &m_stats)},
    m_blurrer{m_goomInfo->GetScreenInfo().width, m_goomInfo->GetScreenInfo().height, 3}
{
#ifndef NO_LOGGING
  Fractal* fractal = fractal.get();
  for (size_t i = 0; i < 5 * maxSimi; i++)
  {
    Similitude cur = fractal->components[i];
    logDebug("simi[{}]: dbl_cx = {:.2}, dbl_cy = {:.2}, r = {:.2}, dbl_r2 = {:.2}, A = {:.2}, A2 = "
             "{:.2}.",
             i, cur.dbl_cx, cur.dbl_cy, cur.dbl_r1, cur.dbl_r2, cur.A1, cur.A2);
  }
#endif
}

IfsDancersFx::IfsDancersFxImpl::~IfsDancersFxImpl() noexcept = default;

void IfsDancersFx::IfsDancersFxImpl::Init()
{
  m_fractal->Init();
  UpdateLowDensityThreshold();
}

inline auto IfsDancersFx::IfsDancersFxImpl::GetResourcesDirectory() const -> const std::string&
{
  return m_resourcesDirectory;
}

inline void IfsDancersFx::IfsDancersFxImpl::SetResourcesDirectory(const std::string& dirName)
{
  m_resourcesDirectory = dirName;
}

inline void IfsDancersFx::IfsDancersFxImpl::SetBuffSettings(const FXBuffSettings& settings)
{
  m_buffSettings = settings;
}

inline void IfsDancersFx::IfsDancersFxImpl::SetSmallImageBitmaps(
    const SmallImageBitmaps& smallBitmaps)
{
  m_fractal->SetSmallImageBitmaps(smallBitmaps);
}

inline auto IfsDancersFx::IfsDancersFxImpl::GetColorMode() const -> IfsDancersFx::ColorMode
{
  return m_colorizer.GetColorMode();
}

inline void IfsDancersFx::IfsDancersFxImpl::SetColorMode(const IfsDancersFx::ColorMode c)
{
  return m_colorizer.SetForcedColorMode(c);
}

inline void IfsDancersFx::IfsDancersFxImpl::Finish()
{
  m_stats.SetlastIfsIncr(m_ifsIncr);
}

inline void IfsDancersFx::IfsDancersFxImpl::Log(const GoomStats::LogStatsValueFunc& l) const
{
  m_stats.Log(l);
}

void IfsDancersFx::IfsDancersFxImpl::Renew()
{
  ChangeColormaps();
  m_colorizer.ChangeColorMode();
  UpdateAllowOverexposed();

  constexpr float MIN_SPEED_AMP = 1.1;
  constexpr float MAX_SPEED_AMP = 5.1;
  constexpr float MAX_SPEED_WEIGHT = 10.0;
  const float speedAmp = std::min(GetRandInRange(MIN_SPEED_AMP, MAX_SPEED_WEIGHT), MAX_SPEED_AMP);
  const float accelFactor = 1.0F / (1.2F - m_goomInfo->GetSoundInfo().GetAcceleration());

  m_fractal->SetSpeed(std::max(1U, static_cast<uint32_t>(speedAmp * accelFactor)));
}

void IfsDancersFx::IfsDancersFxImpl::ChangeColormaps()
{
  m_colorizer.ChangeColorMaps();
}

void IfsDancersFx::IfsDancersFxImpl::ApplyNoDraw()
{
  UpdateDecayAndRecay();
  UpdateDecay();
}

void IfsDancersFx::IfsDancersFxImpl::UpdateIfs(PixelBuffer& currentBuff, PixelBuffer& nextBuff)
{
  m_stats.UpdateStart();

  UpdateDecayAndRecay();

  if (GetIfsIncr() <= 0)
  {
    return;
  }

  // TODO: trouver meilleur soluce pour increment (mettre le code de gestion de l'ifs
  //       dans ce fichier)
  //       find the best solution for increment (put the management code of the ifs in this file)
  m_cycle++;
  if (m_cycle >= CYCLE_LENGTH)
  {
    m_cycle = 0;
    m_stats.UpdateCycleChanges();

    if (ProbabilityOfMInN(15, 20))
    {
      m_lowDensityBlurThreshold = 0.99;
      m_stats.UpdateHighLowDensityBlurThreshold();
    }
    else
    {
      m_lowDensityBlurThreshold = 0.40;
      m_stats.UpdateLowLowDensityBlurThreshold();
    }

    m_fractal->ResetCurrentIfsFunc();

    Renew();
  }

  const std::vector<IfsPoint>& points = m_fractal->DrawIfs();
  const uint32_t maxHitCount = m_fractal->GetMaxHitCount();

  UpdatePixelBuffers(currentBuff, nextBuff, points, maxHitCount);

  m_stats.UpdateEnd();
}

void IfsDancersFx::IfsDancersFxImpl::UpdateIncr()
{
  if (m_ifsIncr <= 0)
  {
    m_recayIfs = 5;
    m_ifsIncr = 11;
    m_stats.UpdateIfsIncr(m_ifsIncr);
    Renew();
  }
}

void IfsDancersFx::IfsDancersFxImpl::UpdateDecay()
{
  if ((m_ifsIncr > 0) && (m_decayIfs <= 0))
  {
    m_decayIfs = 100;
  }
}

void IfsDancersFx::IfsDancersFxImpl::UpdateDecayAndRecay()
{
  m_decayIfs--;
  if (m_decayIfs > 0)
  {
    m_ifsIncr += 2;
  }
  if (m_decayIfs == 0)
  {
    m_ifsIncr = 0;
  }

  if (m_recayIfs)
  {
    m_ifsIncr -= 2;
    m_recayIfs--;
    if ((m_recayIfs == 0) && (m_ifsIncr <= 0))
    {
      m_ifsIncr = 1;
    }
  }

  m_stats.UpdateIfsIncr(m_ifsIncr);
}

inline auto IfsDancersFx::IfsDancersFxImpl::GetIfsIncr() const -> int
{
  return m_ifsIncr;
}

inline void IfsDancersFx::IfsDancersFxImpl::DrawPixel(PixelBuffer& currentBuff,
                                                      PixelBuffer& nextBuff,
                                                      const IfsPoint& point,
                                                      const float tMix)
{
  const float fx =
      static_cast<float>(point.x) / static_cast<float>(m_goomInfo->GetScreenInfo().width);
  const float fy =
      static_cast<float>(point.y) / static_cast<float>(m_goomInfo->GetScreenInfo().height);

  const Pixel mixedColor = m_colorizer.GetMixedColor(point.color, point.count, tMix, fx, fy);
  const std::vector<Pixel> colors{mixedColor, mixedColor};
  std::vector<PixelBuffer*> buffs{&currentBuff, &nextBuff};

  static uint64_t s_count = 0;
  s_count++;

  if (s_count % 50 != 0 || point.simi->currentPointBitmap == nullptr)
  {
    m_draw.SetPixelRgb(buffs, point.x, point.y, colors);
  }
  else
  {
    const auto getColor1 = [&]([[maybe_unused]] const size_t x, [[maybe_unused]] const size_t y,
                               [[maybe_unused]] const Pixel& b) -> Pixel { return mixedColor; };
    const auto getColor2 = [&]([[maybe_unused]] const size_t x, [[maybe_unused]] const size_t y,
                               const Pixel& b) -> Pixel { return b; };
    const std::vector<GoomDraw::GetBitmapColorFunc> getColors{getColor1, getColor2};
    const std::vector<const PixelBuffer*> bitmaps{point.simi->currentPointBitmap,
                                                  point.simi->currentPointBitmap};
    const auto allowOverexposed = m_draw.GetAllowOverexposed();
    m_draw.SetAllowOverexposed(false);
    m_draw.Bitmap(buffs, static_cast<int32_t>(point.x), static_cast<int32_t>(point.y), bitmaps,
                  getColors);
    m_draw.SetAllowOverexposed(allowOverexposed);
  }
}

void IfsDancersFx::IfsDancersFxImpl::UpdateAllowOverexposed()
{
  if (m_buffSettings.allowOverexposed)
  {
    return;
  }

  if (m_allowOverexposed)
  {
    if (m_countSinceOverexposed == 0)
    {
      m_allowOverexposed = false;
    }
    else
    {
      m_countSinceOverexposed--;
    }
  }
  else if (AllowOverexposedEvent())
  {
    m_allowOverexposed = true;
    m_countSinceOverexposed = MAX_COUNT_SINCE_OVEREXPOSED;
  }
}

void IfsDancersFx::IfsDancersFxImpl::UpdatePixelBuffers(PixelBuffer& currentBuff,
                                                        PixelBuffer& nextBuff,
                                                        const std::vector<IfsPoint>& points,
                                                        const uint32_t maxHitCount)
{
  m_colorizer.SetMaxHitCount(maxHitCount);
  bool doneColorChange =
      m_colorizer.GetColorMode() != IfsDancersFx::ColorMode::MEGA_MAP_COLOR_CHANGE &&
      m_colorizer.GetColorMode() != IfsDancersFx::ColorMode::MEGA_MIX_COLOR_CHANGE;
  const size_t numPoints = points.size();
  const float tStep = numPoints == 1 ? 0.0F : (1.0F - 0.0F) / static_cast<float>(numPoints - 1);
  float t = -tStep;

  uint32_t maxLowDensityCount = 0;
  uint32_t numSelectedPoints = 0;
  std::vector<IfsPoint> lowDensityPoints{};
  for (size_t i = 0; i < numPoints; i += static_cast<size_t>(GetIfsIncr()))
  {
    t += tStep;

    const uint32_t x = points[i].x;
    const uint32_t y = points[i].y;
    if ((x >= m_goomInfo->GetScreenInfo().width) || (y >= m_goomInfo->GetScreenInfo().height))
    {
      continue;
    }

    if (!doneColorChange && MegaChangeColorMapEvent())
    {
      ChangeColormaps();
      doneColorChange = true;
    }

    numSelectedPoints++;
    DrawPixel(currentBuff, nextBuff, points[i], t);

    if (points[i].count <= m_lowDensityCount)
    {
      (void)lowDensityPoints.emplace_back(points[i]);
      if (maxLowDensityCount < points[i].count)
      {
        maxLowDensityCount = points[i].count;
      }
    }
  }

  std::vector<PixelBuffer*> buffs{&currentBuff, &nextBuff};
  if (BlurLowDensityColors(numSelectedPoints, lowDensityPoints))
  {
    // Enough dense points to make blurring worthwhile.
    m_blurrer.DoBlur(lowDensityPoints, buffs);
  }
  else
  {
    SetLowDensityColors(lowDensityPoints, maxLowDensityCount, buffs);
  }
}

inline auto IfsDancersFx::IfsDancersFxImpl::BlurLowDensityColors(
    size_t numPoints, const std::vector<IfsPoint>& lowDensityPoints) const -> bool
{
  if (numPoints == 0)
  {
    return false;
  }
  return static_cast<float>(lowDensityPoints.size()) / static_cast<float>(numPoints) >
         m_lowDensityBlurThreshold;
}

void IfsDancersFx::IfsDancersFxImpl::SetLowDensityColors(std::vector<IfsPoint>& points,
                                                         const uint32_t maxLowDensityCount,
                                                         std::vector<PixelBuffer*>& buffs) const
{
  const std::function<Pixel(const IfsPoint&)> getRandomColor =
      [&]([[maybe_unused]] const IfsPoint& p) {
        const float t = GetRandInRange(0.0F, 1.0F);
        return m_colorizer.GetColorMaps().GetRandomColorMap().GetColor(t);
      };

  const IColorMap& colorMap = m_colorizer.GetColorMaps().GetRandomColorMap();
  const float reciprocalMaxLowDensityCount = 1.0F / static_cast<float>(maxLowDensityCount);
  const std::function<Pixel(const IfsPoint&)> getSmoothColor = [&](const IfsPoint& p) {
    const float t = reciprocalMaxLowDensityCount * static_cast<float>(p.count);
    return colorMap.GetColor(t);
  };

  const std::function<Pixel(const IfsPoint&)> getColor =
      m_useRandomLowDensityColors ? getRandomColor : getSmoothColor;

  for (const auto& p : points)
  {
    const Pixel color = getColor(p);
    for (auto& b : buffs)
    {
      (*b)(p.x, p.y) = color;
    }
  }
}

void IfsDancersFx::IfsDancersFxImpl::UpdateLowDensityThreshold()
{
  m_lowDensityCount = GetRandInRange(MIN_DENSITY_COUNT, MAX_DENSITY_COUNT);

  uint32_t blurWidth;
  constexpr uint32_t NUM_WIDTHS = 3;
  constexpr uint32_t WIDTH_RANGE = (MAX_DENSITY_COUNT - MIN_DENSITY_COUNT) / NUM_WIDTHS;
  if (m_lowDensityCount <= MIN_DENSITY_COUNT + WIDTH_RANGE)
  {
    blurWidth = 7;
  }
  else if (m_lowDensityCount <= MIN_DENSITY_COUNT + 2 * WIDTH_RANGE)
  {
    blurWidth = 5;
  }
  else
  {
    blurWidth = 3;
  }
  m_blurrer.SetWidth(blurWidth);
}

} // namespace GOOM
