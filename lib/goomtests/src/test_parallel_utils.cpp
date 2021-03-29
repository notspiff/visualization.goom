#include "catch2/catch.hpp"
#include "parallel_utils.h"

#include <memory>
#include <set>
#include <thread>
#include <vector>

using GOOM::UTILS::Parallel;

TEST_CASE("Test Parallel Utils", "[ParallelFor]")
{
  std::set<std::thread::id> threadsUsed{};
  std::mutex mutex{};

  constexpr size_t ARRAY_LEN = 100000;
  std::vector<uint64_t> testArray(ARRAY_LEN);
  constexpr uint64_t FIXED_VAL = 33;
  const auto f = [](const uint64_t i) { return i * FIXED_VAL + i * i; };
  const auto assignF = [&testArray, &f, &threadsUsed, &mutex](const uint64_t i) {
    testArray[i] = f(i);
    std::lock_guard<std::mutex> l(mutex);
    (void)threadsUsed.emplace(std::this_thread::get_id());
  };

  const auto checkResults = [&testArray, &threadsUsed, &f](const Parallel& parallel,
                                                           const uint32_t numThreadsUsed) {
    for (size_t i = 0; i < ARRAY_LEN; ++i)
    {
      REQUIRE(testArray[i] == f(i));
    }
    REQUIRE(parallel.GetNumThreadsUsed() == numThreadsUsed);
    REQUIRE(threadsUsed.size() == numThreadsUsed);
  };

  auto parallel = std::make_unique<Parallel>(-1);
  auto numThreadsUsed = static_cast<uint32_t>(std::thread::hardware_concurrency() - 1);
  threadsUsed.clear();
  parallel->ForLoop(ARRAY_LEN, assignF);
  checkResults(*parallel, numThreadsUsed);

  parallel = std::make_unique<Parallel>(-2);
  numThreadsUsed = static_cast<uint32_t>(std::thread::hardware_concurrency() - 2);
  threadsUsed.clear();
  parallel->ForLoop(ARRAY_LEN, assignF);
  checkResults(*parallel, numThreadsUsed);

  parallel = std::make_unique<Parallel>(1);
  numThreadsUsed = 1;
  threadsUsed.clear();
  parallel->ForLoop(ARRAY_LEN, assignF);
  checkResults(*parallel, numThreadsUsed);
}
