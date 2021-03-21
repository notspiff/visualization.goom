#include "thread_pool.h"

#include <cassert>
#include <functional>
#include <thread>
#include <utility>

#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

ThreadPool::ThreadPool(const size_t numWorkers) noexcept : m_numWorkers(numWorkers)
{
  assert(m_numWorkers > 0);
  m_workers.reserve(m_numWorkers);
  for (size_t i = 0; i < m_numWorkers; ++i)
  {
    m_workers.emplace_back(&ThreadPool::ThreadLoop, this);
  }
}

ThreadPool::~ThreadPool() noexcept
{
  {
    const std::lock_guard<std::mutex> lock{m_mutex};
    m_finished = true;
  }
  // Tell all the workers we're ready.
  m_newWorkCondition.notify_all();

  for (std::thread& thread : m_workers)
  {
    thread.join();
  }
}

auto ThreadPool::GetThreadIds() const -> std::vector<std::thread::id>
{
  std::vector<std::thread::id> threadIds{};
  for (const std::thread& thread : m_workers)
  {
    threadIds.emplace_back(thread.get_id());
  }
  return threadIds;
}

auto ThreadPool::GetOutstandingWorkSize() const -> size_t
{
  const std::lock_guard<std::mutex> lock{m_mutex};
  return m_workQueue.size();
}

auto ThreadPool::GetNumWorkers() const -> size_t
{
  return m_numWorkers;
}

void ThreadPool::SetWorkDoneCallback(std::function<void(int)> func)
{
  m_workDoneCallback = std::move(func);
}

void ThreadPool::Wait()
{
  std::unique_lock<std::mutex> lock{m_mutex};
  if (!m_workQueue.empty())
  {
    m_workDoneCondition.wait(lock, [this] { return m_workQueue.empty(); });
  }
}

void ThreadPool::ThreadLoop()
{
  // Wait until the ThreadPool sends us work.
  while (true)
  {
    WorkItem workItem{};

    int prevWorkSize = -1;
    {
      std::unique_lock<std::mutex> lock{m_mutex};
      m_newWorkCondition.wait(lock, [this] { return m_finished || (!m_workQueue.empty()); });

      // If all the work is done and exit_ is true, then break out of the loop.
      if (m_finished && m_workQueue.empty())
      {
        break;
      }

      // Pop the work off of the queue - we are careful to execute the
      // work_item.func callback only after we have released the lock.
      prevWorkSize = m_workQueue.size();
      workItem = std::move(m_workQueue.front());
      m_workQueue.pop();
    }

    // We are careful to do the work without the lock held!
    workItem.func(); // Do work.

    if (m_workDoneCallback)
    {
      m_workDoneCallback(prevWorkSize - 1);
    }

    // Notify if all work is done.
    {
      const std::unique_lock<std::mutex> lock{m_mutex};
      if (m_workQueue.empty() && prevWorkSize == 1)
      {
        m_workDoneCondition.notify_all();
      }
    }
  }
}

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
