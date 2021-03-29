#ifndef VISUALIZATION_GOOM_LIB_GOOMUTILS_GOOMRAND_H_
#define VISUALIZATION_GOOM_LIB_GOOMUTILS_GOOMRAND_H_

#include <cmath>
#include <cstdlib>
#include <format>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>


#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

auto GetRandSeed() -> uint64_t;
void SetRandSeed(uint64_t seed);
extern const uint32_t g_randMax;

void SaveRandState(std::ostream& f);
void RestoreRandState(std::istream& f);

// Return random sign integer, either -1 or +1.
inline auto GetRandSignInt() -> int;
// Return random sign float, either -1.0 or +1.0.
inline auto GetRandSignFlt() -> float;

// Return random positive integer in the range n0 <= n < n1.
auto GetRandInRange(uint32_t n0, uint32_t n1) -> uint32_t;
// Return random integer in the range 0 <= n < nmax.
auto GetNRand(uint32_t nmax) -> uint32_t;
// Return random integer in the range 0 <= n < randMax.
auto GetRand() -> uint32_t;
// Return random integer in the range n0 <= n < n1.
auto GetRandInRange(int32_t n0, int32_t n1) -> int32_t;
// Return random float in the range x0 <= n <= x1.
auto GetRandInRange(float x0, float x1) -> float;

// Return prob(m/n)
inline auto ProbabilityOfMInN(uint32_t m, uint32_t n) -> bool;
inline auto ProbabilityOf(float p) -> bool;

template<class E>
class Weights
{
public:
  Weights() noexcept;
  explicit Weights(const std::vector<std::pair<E, size_t>>& w);

  void ClearWeights(size_t value);
  [[nodiscard]] auto GetNumElements() const -> size_t;
  void SetWeight(E e, size_t value);
  auto GetWeight(E e) const -> size_t;

  [[nodiscard]] auto GetSumOfWeights() const -> size_t { return m_sumOfWeights; }

  auto GetRandomWeighted() const -> E;

private:
  std::vector<std::pair<E, size_t>> m_weights{};
  size_t m_sumOfWeights{};
  static auto GetSumOfWeights(const std::vector<std::pair<E, size_t>>& w) -> size_t;
};

inline auto GetRandSignInt() -> int
{
  return GetRandInRange(0U, 100U) < 50 ? -1 : +1;
}

inline auto GetRandSignFlt() -> float
{
  return GetRandInRange(0U, 100U) < 50 ? -1.0F : +1.0F;
}

inline auto GetNRand(const uint32_t nmax) -> uint32_t
{
  return GetRandInRange(0U, nmax);
}

inline auto GetRand() -> uint32_t
{
  return GetRandInRange(0U, g_randMax);
}

inline auto ProbabilityOfMInN(const uint32_t m, const uint32_t n) -> bool
{
  if (m == 1)
  {
    return GetNRand(n) == 0;
  }
  if (m == n - 1)
  {
    return GetNRand(n) > 0;
  }
  return GetRandInRange(0.0F, 1.0F) <= static_cast<float>(m) / static_cast<float>(n);
}

inline auto ProbabilityOf(const float p) -> bool
{
  return GetRandInRange(0.0F, 1.0F) <= p;
}

template<class E>
Weights<E>::Weights() noexcept = default;

template<class E>
Weights<E>::Weights(const std::vector<std::pair<E, size_t>>& w)
  : m_weights{w}, m_sumOfWeights{GetSumOfWeights(w)}
{
}

template<class E>
auto Weights<E>::GetSumOfWeights(const std::vector<std::pair<E, size_t>>& weights) -> size_t
{
  size_t sumOfWeights = 0;
#if __cplusplus <= 201402L
  for (const auto& wgt : weights)
  {
    const auto& w = std::get<1>(wgt);
#else
    for (const auto& [e, w] : weights)
  {
#endif
    sumOfWeights += w;
  }
  return sumOfWeights;
}

template<class E>
auto Weights<E>::GetNumElements() const -> size_t
{
  return m_weights.size();
}

template<class E>
void Weights<E>::SetWeight(const E enumClass, const size_t value)
{
#if __cplusplus <= 201402L
  for (auto& wgt : m_weights)
  {
    const auto& e = std::get<0>(wgt);
    auto& w = std::get<1>(wgt);
#else
  for (auto& [e, w] : m_weights)
  {
#endif
    if (e == enumClass)
    {
      w = value;
      m_sumOfWeights = GetSumOfWeights(m_weights);
      return;
    }
  }
  m_weights.emplace_back(std::make_pair(enumClass, value));
  m_sumOfWeights = GetSumOfWeights(m_weights);
}

template<class E>
auto Weights<E>::GetWeight(const E enumClass) const -> size_t
{
#if __cplusplus <= 201402L
  for (const auto& wgt : m_weights)
  {
    const auto& e = std::get<0>(wgt);
    const auto& w = std::get<1>(wgt);
#else
    for (const auto& [e, w] : m_weights)
  {
#endif
    if (e == enumClass)
    {
      return w;
    }
  }
  return 0;
}

template<class E>
void Weights<E>::ClearWeights(const size_t value)
{
#if __cplusplus <= 201402L
  for (auto& wgt : m_weights)
  {
    auto& w = std::get<1>(wgt);
#else
  for (auto& [e, w] : m_weights)
  {
#endif
    w = value;
  }
  m_sumOfWeights = GetSumOfWeights(m_weights);
}

template<class E>
auto Weights<E>::GetRandomWeighted() const -> E
{
  if (m_weights.empty())
  {
    throw std::logic_error("The are no weights set.");
  }

  uint32_t randVal = GetRandInRange(0U, m_sumOfWeights);
#if __cplusplus <= 201402L
  for (const auto& wgt : m_weights)
  {
    const auto& e = std::get<0>(wgt);
    const auto& w = std::get<1>(wgt);
#else
  for (const auto& [e, w] : m_weights)
  {
#endif
    if (randVal < w)
    {
      return e;
    }
    randVal -= w;
  }
  throw std::logic_error(std20::format("Should not get here. randVal = {}.", randVal));
}

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
#endif
