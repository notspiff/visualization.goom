#include "filter_buffers.h"

#include "goom_graphic.h"
#include "goom_plugin_info.h"
#include "goomutils/goomrand.h"
#include "goomutils/logging_control.h"
//#undef NO_LOGGING
#include "goomutils/logging.h"
#include "goomutils/parallel_utils.h"
#include "v2d.h"

#include <array>
#undef NDEBUG
#include <cassert>
#include <cmath>
#include <cstdint>
#include <tuple>
#include <vector>

namespace GOOM
{

using namespace GOOM::UTILS;

// clang linking trouble - should be able to use static constexpr
const float ZoomFilterBuffers::MAX_NORMALIZED_COORD = 2.0;
const float ZoomFilterBuffers::MIN_NORMALIZED_COORD = -MAX_NORMALIZED_COORD;

class ZoomFilterBuffers::FilterCoefficients
{
public:
  FilterCoefficients() noexcept;
  virtual ~FilterCoefficients() noexcept = default;
  FilterCoefficients(const FilterCoefficients&) noexcept = delete;
  FilterCoefficients(FilterCoefficients&&) noexcept = delete;
  auto operator=(const FilterCoefficients&) -> FilterCoefficients& = delete;
  auto operator=(FilterCoefficients&&) -> FilterCoefficients& = delete;

  auto GetCoeffs() const -> const FilterCoeff2dArray&;

private:
  // modif d'optim by Jeko : precalcul des 4 coefs resultant des 2 pos
  const FilterCoeff2dArray m_precalculatedCoeffs;
  static auto GetPrecalculatedCoefficients() -> FilterCoeff2dArray;
};

ZoomFilterBuffers::FilterCoefficients::FilterCoefficients() noexcept
  : m_precalculatedCoeffs{GetPrecalculatedCoefficients()}
{
}

inline auto ZoomFilterBuffers::FilterCoefficients::GetCoeffs() const -> const FilterCoeff2dArray&
{
  return m_precalculatedCoeffs;
}

auto ZoomFilterBuffers::FilterCoefficients::GetPrecalculatedCoefficients() -> FilterCoeff2dArray
{
  FilterCoeff2dArray precalculatedCoeffs{};

  for (uint32_t coeffH = 0; coeffH < DIM_FILTER_COEFFS; coeffH++)
  {
    for (uint32_t coeffV = 0; coeffV < DIM_FILTER_COEFFS; coeffV++)
    {
      const uint32_t diffCoeffH = DIM_FILTER_COEFFS - coeffH;
      const uint32_t diffCoeffV = DIM_FILTER_COEFFS - coeffV;

      if (coeffH == 0 && coeffV == 0)
      {
        precalculatedCoeffs[coeffH][coeffV].intVal = MAX_COLOR_VAL;
      }
      else
      {
        uint32_t i1 = diffCoeffH * diffCoeffV;
        uint32_t i2 = coeffH * diffCoeffV;
        uint32_t i3 = diffCoeffH * coeffV;
        uint32_t i4 = coeffH * coeffV;

        // TODO: faire mieux...
        if (i1)
        {
          i1--;
        }
        if (i2)
        {
          i2--;
        }
        if (i3)
        {
          i3--;
        }
        if (i4)
        {
          i4--;
        }

        /**
        if (ProbabilityOfMInN(1, 100))
        {
          i1 += GetRandInRange(0U, 10U);
        }
        if (ProbabilityOfMInN(1, 100))
        {
          i2 += GetRandInRange(0U, 10U);
        }
        if (ProbabilityOfMInN(1, 100))
        {
          i3 += GetRandInRange(0U, 10U);
        }
        if (ProbabilityOfMInN(1, 100))
        {
          i4 += GetRandInRange(0U, 10U);
        }
        i1 = 16;
        i2 =  20;
        i3 = 20;
        i4 = 16;
**/
        precalculatedCoeffs[coeffH][coeffV] = NeighborhoodCoeffArray{/*.c*/ {
            static_cast<uint8_t>(i1),
            static_cast<uint8_t>(i2),
            static_cast<uint8_t>(i3),
            static_cast<uint8_t>(i4),
        }};
      }
    }
  }

  return precalculatedCoeffs;
}

constexpr int32_t DIM_FILTER_COEFFS = ZoomFilterBuffers::DIM_FILTER_COEFFS;
// For optimising multiplication, division, and mod by DIM_FILTER_COEFFS.
constexpr int32_t DIM_FILTER_COEFFS_DIV_SHIFT = 4;
constexpr int32_t DIM_FILTER_COEFFS_MOD_MASK = 0xF;
constexpr int32_t MAX_TRAN_DIFF_FACTOR = 0xFFFF;

constexpr float MIN_SCREEN_COORD_VAL = 1.0F / static_cast<float>(DIM_FILTER_COEFFS);

inline auto TranToCoeffIndexCoord(const uint32_t tranCoord)
{
  return tranCoord & DIM_FILTER_COEFFS_MOD_MASK;
}

inline auto TranToScreenPoint(const V2dInt& tranPoint) -> V2dInt
{
  return {tranPoint.x >> DIM_FILTER_COEFFS_DIV_SHIFT, tranPoint.y >> DIM_FILTER_COEFFS_DIV_SHIFT};
}

inline auto ScreenToTranPoint(const V2dInt& screenPoint) -> V2dInt
{
  return {screenPoint.x << DIM_FILTER_COEFFS_DIV_SHIFT,
          screenPoint.y << DIM_FILTER_COEFFS_DIV_SHIFT};
}

inline auto ScreenToTranCoord(const float screenCoord) -> uint32_t
{
  // IMPORTANT: Without 'lround' a faint cross artifact appears in the centre of the screen.
  return static_cast<uint32_t>(std::lround(screenCoord * static_cast<float>(DIM_FILTER_COEFFS)));
}

inline auto GetTranBuffLerp(const int32_t srceBuffVal, const int32_t destBuffVal, const int32_t t)
    -> int32_t
{
  return srceBuffVal + ((t * (destBuffVal - srceBuffVal)) >> DIM_FILTER_COEFFS);
}

ZoomFilterBuffers::ZoomFilterBuffers(Parallel& p, const std::shared_ptr<const PluginInfo>& goomInfo)
  : m_screenWidth{goomInfo->GetScreenInfo().width},
    m_screenHeight{goomInfo->GetScreenInfo().height},
    m_bufferSize{goomInfo->GetScreenInfo().size},
    m_precalculatedCoeffs{std::make_unique<FilterCoefficients>()},
    m_ratioScreenToNormalizedCoord{MAX_NORMALIZED_COORD / static_cast<float>(m_screenWidth - 1)},
    m_ratioNormalizedToScreenCoord{1.0F / m_ratioScreenToNormalizedCoord},
    m_minNormalizedCoordVal{GetMinNormalizedCoordVal(m_screenWidth)},
    m_maxNormalizedCoordVal{m_ratioScreenToNormalizedCoord * static_cast<float>(m_screenWidth - 1)},
    m_parallel{&p},
    m_tranLerpFactor{0},
    m_tranXSrce(m_bufferSize),
    m_tranYSrce(m_bufferSize),
    m_tranXDest(m_bufferSize),
    m_tranYDest(m_bufferSize),
    m_tranXTemp(m_bufferSize),
    m_tranYTemp(m_bufferSize),
    m_maxTranPoint{ScreenToTranPoint(
        {static_cast<int32_t>(m_screenWidth - 1), static_cast<int32_t>(m_screenHeight - 1)})},
    m_tranBuffStripeHeight{m_screenHeight / DIM_FILTER_COEFFS},
    m_tranBuffYLineStart{0},
    m_tranBufferState{TranBufferState::TRAN_BUFFER_READY},
    m_firedec(m_screenHeight)
{
  assert(DIM_FILTER_COEFFS ==
         static_cast<int32_t>(std::lround(std::pow(2, DIM_FILTER_COEFFS_DIV_SHIFT))));
  assert(MAX_TRAN_DIFF_FACTOR ==
         static_cast<int32_t>(std::lround(std::pow(2, DIM_FILTER_COEFFS))) - 1);
}

ZoomFilterBuffers::~ZoomFilterBuffers() noexcept = default;

auto ZoomFilterBuffers::GetMinNormalizedCoordVal(const uint32_t screenWidth) -> float
{
  return (MAX_NORMALIZED_COORD / static_cast<float>(screenWidth - 1)) * MIN_SCREEN_COORD_VAL;
}

auto ZoomFilterBuffers::GetZoomVectorFunc() const -> ZoomVectorFunc
{
  return m_zoomVectorFunc;
}

void ZoomFilterBuffers::SetZoomVectorFunc(const ZoomVectorFunc& f)
{
  m_zoomVectorFunc = f;
}

auto ZoomFilterBuffers::GetMaxTranLerpFactor() -> int32_t
{
  return MAX_TRAN_DIFF_FACTOR;
}

void ZoomFilterBuffers::SetTranLerpFactor(int32_t val)
{
  m_tranLerpFactor = val;
}

inline auto ZoomFilterBuffers::NormalizedToTranPoint(const V2dFlt& normalizedPoint) const -> V2dInt
{
  // IMPORTANT: Without 'lround' a faint cross artifact appears in the centre of the screen.
  return {static_cast<int32_t>(
              std::lround(ScreenToTranCoord(NormalizedToScreenCoordFlt(normalizedPoint.x)))),
          static_cast<int32_t>(
              std::lround(ScreenToTranCoord(NormalizedToScreenCoordFlt(normalizedPoint.y))))};
}

inline auto ZoomFilterBuffers::ScreenToNormalizedCoord(const int32_t screenCoord) const -> float
{
  return m_ratioScreenToNormalizedCoord * static_cast<float>(screenCoord);
}

inline auto ZoomFilterBuffers::NormalizedToScreenCoordFlt(const float normalizedCoord) const
    -> float
{
  return m_ratioNormalizedToScreenCoord * normalizedCoord;
}

inline void ZoomFilterBuffers::IncNormalizedCoord(float& normalizedCoord) const
{
  normalizedCoord += m_ratioScreenToNormalizedCoord;
}

void ZoomFilterBuffers::Start()
{
  InitTranBuffer();
}

auto ZoomFilterBuffers::GetZoomBufferSrceDestLerp(const size_t buffPos) const -> V2dInt
{
  return {GetTranBuffLerp(m_tranXSrce[buffPos], m_tranXDest[buffPos], m_tranLerpFactor),
          GetTranBuffLerp(m_tranYSrce[buffPos], m_tranYDest[buffPos], m_tranLerpFactor)};
}

auto ZoomFilterBuffers::IsTranPointClipped(const V2dInt& tranPoint) const -> bool
{
  return (tranPoint.x < 0 || tranPoint.y < 0 ||
          static_cast<uint32_t>(tranPoint.x) >= GetMaxTranX() ||
          static_cast<uint32_t>(tranPoint.y) >= GetMaxTranY());
}

auto ZoomFilterBuffers::GetSourceInfo(const V2dInt& tranPoint) const
    -> std::tuple<V2dInt, NeighborhoodCoeffArray>
{
  const V2dInt srcePoint = TranToScreenPoint(tranPoint);
  const size_t xIndex = TranToCoeffIndexCoord(static_cast<uint32_t>(tranPoint.x));
  const size_t yIndex = TranToCoeffIndexCoord(static_cast<uint32_t>(tranPoint.y));
  return std::make_tuple(srcePoint, m_precalculatedCoeffs->GetCoeffs()[xIndex][yIndex]);
}

void ZoomFilterBuffers::SettingsChanged()
{
  m_settingsChanged = true;
}

void ZoomFilterBuffers::UpdateTranBuffer()
{
  if (m_tranBufferState == TranBufferState::RESET_TRAN_BUFFER)
  {
    //LogState("Before RESET_TRAN_BUFFER");
    ResetTranBuffer();
    //LogState("After RESET_TRAN_BUFFER");
  }
  else if (m_tranBufferState == TranBufferState::RESTART_TRAN_BUFFER)
  {
    //LogState("Before RESTART_TRAN_BUFFER");
    RestartTranBuffer();
    //LogState("After RESTART_TRAN_BUFFER");
  }
  else
  {
    // Create a new destination stripe of 'm_tranBuffStripeHeight' height starting
    // at 'm_tranBuffYLineStart'.
    //LogState("Before DoNextTranBufferStripe");
    DoNextTranBufferStripe(m_tranBuffStripeHeight);
    //LogState("After DoNextTranBufferStripe");
  }
}

void ZoomFilterBuffers::InitTranBuffer()
{
  GenerateWaterFxHorizontalBuffer();
  DoNextTranBufferStripe(m_screenHeight);

  // Identity source tran buffer
  size_t i = 0;
  for (int32_t y = 0; y < static_cast<int32_t>(m_screenHeight); y++)
  {
    for (int32_t x = 0; x < static_cast<int32_t>(m_screenWidth); x++)
    {
      const V2dInt tranSrcePoint = ScreenToTranPoint({x, y});
      m_tranXSrce[i] = tranSrcePoint.x;
      m_tranYSrce[i] = tranSrcePoint.y;
      i++;
    }
  }

  // Copy temp tran to dest tran.
  std::copy(m_tranXTemp.begin(), m_tranXTemp.end(), m_tranXDest.begin());
  std::copy(m_tranYTemp.begin(), m_tranYTemp.end(), m_tranYDest.begin());

  m_tranBuffYLineStart = 0;
  m_tranBufferState = TranBufferState::RESTART_TRAN_BUFFER;
}

void ZoomFilterBuffers::ResetTranBuffer()
{
  // generation du buffer de transform

  // sauvegarde de l'etat actuel dans la nouvelle source
  // Save the current state in the source buffs.
  if (m_tranLerpFactor == 0)
  {
    // Nothing to do: tran srce == tran dest.
  }
  else if (m_tranLerpFactor == MAX_TRAN_DIFF_FACTOR)
  {
    std::copy(m_tranXDest.begin(), m_tranXDest.end(), m_tranXSrce.begin());
    std::copy(m_tranYDest.begin(), m_tranYDest.end(), m_tranYSrce.begin());
  }
  else
  {
    for (size_t i = 0; i < m_bufferSize; i++)
    {
      const V2dInt tranSrcePoint = GetZoomBufferSrceDestLerp(i);
      m_tranXSrce[i] = tranSrcePoint.x;
      m_tranYSrce[i] = tranSrcePoint.y;
    }
  }

  // Set up the next dest buffs from the last buffer stripes.
  std::swap(m_tranXDest, m_tranXTemp);
  std::swap(m_tranYDest, m_tranYTemp);

  m_tranLerpFactor = 0;
  m_tranBuffYLineStart = 0;
  m_tranBufferState = TranBufferState::RESTART_TRAN_BUFFER;
}

void ZoomFilterBuffers::RestartTranBuffer()
{
  if (!m_settingsChanged)
  {
    return;
  }

  m_settingsChanged = false;
  m_tranBuffYLineStart = 0;
  m_tranBufferState = TranBufferState::TRAN_BUFFER_READY;
}

/*
 * Makes a stripe of a transform buffer
 *
 * The transform is (in order) :
 * Translation (-data->middleX, -data->middleY)
 * Homothetie (Center : 0,0   Coeff : 2/data->screenWidth)
 */
void ZoomFilterBuffers::DoNextTranBufferStripe(const uint32_t tranBuffStripeHeight)
{
  assert(m_tranBufferState == TranBufferState::TRAN_BUFFER_READY);

  const float xMidNormalized = ScreenToNormalizedCoord(m_buffMidPoint.x);
  const float yMidNormalized = ScreenToNormalizedCoord(m_buffMidPoint.y);

  const auto doStripeLine = [&](const uint32_t y) {
    // Position of the pixel to compute in screen coordinates
    const uint32_t yOffset = y + m_tranBuffYLineStart;
    const uint32_t tranPosStart = yOffset * m_screenWidth;

    const float yNormalized =
        ScreenToNormalizedCoord(static_cast<int32_t>(yOffset)) - yMidNormalized;
    float xNormalized = -xMidNormalized;

    for (uint32_t x = 0; x < m_screenWidth; x++)
    {
      const V2dFlt zoomVector = m_zoomVectorFunc(xNormalized, yNormalized);
      LogDebug("zoomVector = ({}, {})", zoomVector.x, zoomVector.y);

      const V2dFlt recenteredZoomPoint = {xMidNormalized + xNormalized - zoomVector.x,
                                          yMidNormalized + yNormalized - zoomVector.y};
      const auto tranPoint = GetTranPoint(recenteredZoomPoint);
      const uint32_t tranPos = tranPosStart + x;
      m_tranXTemp[tranPos] = tranPoint.x;
      m_tranYTemp[tranPos] = tranPoint.y;

      IncNormalizedCoord(xNormalized);
    }
  };

  // Where (vertically) to stop generating the buffer stripe
  const uint32_t tranBuffYLineEnd =
      std::min(m_screenHeight, m_tranBuffYLineStart + tranBuffStripeHeight);

  m_parallel->ForLoop(tranBuffYLineEnd - static_cast<uint32_t>(m_tranBuffYLineStart), doStripeLine);

  m_tranBuffYLineStart += tranBuffStripeHeight;
  if (tranBuffYLineEnd >= m_screenHeight)
  {
    m_tranBufferState = TranBufferState::RESET_TRAN_BUFFER;
    m_tranBuffYLineStart = 0;
  }
}

inline auto ZoomFilterBuffers::GetTranPoint(const V2dFlt& normalized) const -> V2dInt
{
  return NormalizedToTranPoint(normalized);

  /**
  int32_t tranX = NormalizedToTranPoint(xNormalised);
  if (tranX < 0)
  {
    tranX = -1;
  }
  else if (tranX >= static_cast<int32_t>(m_maxTranX))
  {
    //    tranX = static_cast<int32_t>(m_maxTranX - 1);
    tranX = static_cast<int32_t>(m_maxTranX);
  }

  int32_t tranY = NormalizedToTranPoint(yNormalised);
  if (tranY < 0)
  {
    tranY = -1;
  }
  else if (tranY >= static_cast<int32_t>(m_maxTranY))
  {
    //    tranY = static_cast<int32_t>(m_maxTranY - 1);
    tranY = static_cast<int32_t>(m_maxTranY);
  }

  return std::make_tuple(tranX, tranY);
   **/
}

void ZoomFilterBuffers::GenerateWaterFxHorizontalBuffer()
{
  int32_t decc = GetRandInRange(-4, +4);
  int32_t spdc = GetRandInRange(-4, +4);
  int32_t accel = GetRandInRange(-4, +4);

  for (size_t loopv = m_screenHeight; loopv != 0;)
  {
    loopv--;
    m_firedec[loopv] = decc;
    decc += spdc / 10;
    spdc += GetRandInRange(-2, +3);

    if (decc > 4)
    {
      spdc -= 1;
    }
    if (decc < -4)
    {
      spdc += 1;
    }

    if (spdc > 30)
    {
      spdc = spdc - static_cast<int32_t>(GetNRand(3)) + accel / 10;
    }
    if (spdc < -30)
    {
      spdc = spdc + static_cast<int32_t>(GetNRand(3)) + accel / 10;
    }

    if (decc > 8 && spdc > 1)
    {
      spdc -= GetRandInRange(-2, +1);
    }
    if (decc < -8 && spdc < -1)
    {
      spdc += static_cast<int32_t>(GetNRand(3)) + 2;
    }
    if (decc > 8 || decc < -8)
    {
      decc = decc * 8 / 9;
    }

    accel += GetRandInRange(-1, +2);
    if (accel > 20)
    {
      accel -= 2;
    }
    if (accel < -20)
    {
      accel += 2;
    }
  }
}

} // namespace GOOM
