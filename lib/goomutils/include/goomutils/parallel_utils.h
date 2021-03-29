#ifndef VISUALIZATION_GOOM_PARALLEL_UTILS_H
#define VISUALIZATION_GOOM_PARALLEL_UTILS_H

#include "thread_pool.h"

#include <algorithm>
#include <cstdint>
#include <future>
#include <thread>
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

class Parallel
{
public:
  // numPoolThreads > 0:  use this number of threads in the pool
  // numPoolThreads <= 0: use (max cores - numPoolThreads) in the pool
  explicit Parallel(int32_t numPoolThreads = 0) noexcept;
  ~Parallel() noexcept = default;
  Parallel(const Parallel&) = delete;
  Parallel(Parallel&&) = delete;
  auto operator=(const Parallel&) -> Parallel& = delete;
  auto operator=(Parallel&&) -> Parallel& = delete;

  auto GetNumThreadsUsed() const -> size_t;
  auto GetThreadIds() const -> std::vector<std::thread::id>;

  template<typename Callable>
  void ForLoop(uint32_t numIters, Callable loopFunc);

private:
  ThreadPool m_threadPool;
};

inline Parallel::Parallel(const int32_t numPoolThreads) noexcept
  : m_threadPool{
        (numPoolThreads <= 0)
            ? static_cast<size_t>(std::max(
                  1, static_cast<int32_t>(std::thread::hardware_concurrency()) + numPoolThreads))
            : static_cast<size_t>(std::min(
                  numPoolThreads, static_cast<int32_t>(std::thread::hardware_concurrency())))}
{
}

inline auto Parallel::GetNumThreadsUsed() const -> size_t
{
  return m_threadPool.GetNumWorkers();
}

inline auto Parallel::GetThreadIds() const -> std::vector<std::thread::id>
{
  return m_threadPool.GetThreadIds();
}

template<typename Callable>
void Parallel::ForLoop(uint32_t numIters, const Callable loopFunc)
{
  if (numIters == 0)
  {
    throw std::logic_error("ForLoop: numIters == 0.");
  }

  const uint32_t numThreads =
      std::min(numIters, static_cast<uint32_t>(m_threadPool.GetNumWorkers()));

  if (numThreads == 1)
  {
    for (uint32_t i = 0; i < numIters; i++)
    {
      loopFunc(i);
    }
    return;
  }

  const uint32_t chunkSize = numIters / numThreads; // >= 1
  const uint32_t numLeftoverIters = numIters - numThreads * chunkSize;

  const auto loopContents = [&](const uint32_t threadIndex) {
    const uint32_t inclusiveStartIndex = threadIndex * chunkSize;
    const uint32_t exclusiveEndIndex =
        inclusiveStartIndex + chunkSize + (threadIndex < numThreads - 1 ? 0 : numLeftoverIters);

    for (uint32_t k = inclusiveStartIndex; k < exclusiveEndIndex; k++)
    {
      loopFunc(k);
    }
  };

  std::vector<std::future<void>> futures{};
  for (uint32_t j = 0; j < numThreads; j++)
  {
    futures.emplace_back(m_threadPool.ScheduleAndGetFuture(loopContents, j));
  }

  for (auto& f : futures)
  {
    f.wait();
  }
}

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
#endif
