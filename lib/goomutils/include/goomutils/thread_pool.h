#ifndef VISUALIZATION_GOOM_THREAD_POOL_H
#define VISUALIZATION_GOOM_THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
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

#if __cplusplus <= 201402L
#define INVOKE_MACRO(CALLABLE, ARGS_TYPE, ARGS) CALLABLE(std::forward<ARGS_TYPE>(ARGS)...)
#else
#define INVOKE_MACRO(CALLABLE, ARGS_TYPE, ARGS) \
  std::invoke(CALLABLE, std::forward<ARGS_TYPE>(ARGS)...)
#endif

class ThreadPool
{
public:
  ThreadPool() noexcept = delete;
  explicit ThreadPool(size_t numWorkers) noexcept;
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) noexcept = delete;
  auto operator=(const ThreadPool&) -> ThreadPool& = delete;
  auto operator=(ThreadPool&&) noexcept -> ThreadPool& = delete;

  // The destructor blocks until all outstanding work is complete.
  ~ThreadPool() noexcept;

  //   Given a callable 'func' that returns a value of type 'RetT', this
  //   function returns a std::future<RetT> that can be used to access
  //   func's results.
  template<typename FuncT, typename... ArgsT>
  auto ScheduleAndGetFuture(FuncT&& func, ArgsT&&... args)
      -> std::future<decltype(INVOKE_MACRO(func, ArgsT, args))>;

  // Wait for all outstanding work to be completed.
  void Wait();

  auto GetNumWorkers() const -> size_t;
  auto GetThreadIds() const -> std::vector<std::thread::id>;

  auto GetOutstandingWorkSize() const -> size_t;
  void SetWorkDoneCallback(std::function<void(int)> func);

private:
  size_t m_numWorkers = 0;

  // The destructor sets 'finished' to true and then notifies all workers.
  // 'finished' causes each thread to break out of their work loop.
  bool m_finished = false;

  // Work queue. Guarded by 'm_mutex'.
  mutable std::mutex m_mutex{};
  struct WorkItem
  {
    std::function<void(void)> func{};
  };
  std::queue<WorkItem> m_workQueue{};

  std::vector<std::thread> m_workers{};
  std::condition_variable m_newWorkCondition{};
  std::condition_variable m_workDoneCondition{};

  // Whenever a work item is complete, we call this callback. If this is empty,
  // nothing is done.
  std::function<void(int)> m_workDoneCallback{};

  void ThreadLoop();
};

namespace THREAD_POOL_IMPL
{

// This helper class simply returns a std::function that executes:
//
//   ReturnT x = func();
//   promise->set_value(x);
//
// However, this is tricky in the case where T == void. The code above won't
// compile if ReturnT == void, and neither will
//
//   promise->set_value(func());
//
// To workaround this, we use a template specialization for the case where ReturnT
// is void. If the "regular void" proposal is accepted, this could be simpler:
//   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0146r1.html.

// The following non-specialized 'FuncWrapper' handles callables that return a
// non-void value.
template<typename ReturnT>
struct FuncWrapper
{
  template<typename FuncT, typename... ArgsT>
  auto GetWrapped(FuncT&& func, std::shared_ptr<std::promise<ReturnT>> promise, ArgsT&&... args)
      -> std::function<void()>
  {
    // Capturing by value is inefficient. It would be more efficient to move-capture
    // everything - can generalized lambda capture help??
    return
        [promise, func, args...]() mutable { promise->set_value(INVOKE_MACRO(func, ArgsT, args)); };
  }
};

template<typename FuncT, typename... ArgsT>
void InvokeVoidRet(FuncT&& func,
                   const std::shared_ptr<std::promise<void>>& promise,
                   ArgsT&&... args)
{
  INVOKE_MACRO(func, ArgsT, args);
  promise->set_value();
}

// Specialization to handle callables that return void.
template<>
struct FuncWrapper<void>
{
  template<typename FuncT, typename... ArgsT>
  auto GetWrapped(FuncT&& func, const std::shared_ptr<std::promise<void>>& promise, ArgsT&&... args)
      -> std::function<void()>
  {
    return [promise, func, args...]() mutable {
      INVOKE_MACRO(func, ArgsT, args);
      promise->set_value();
    };
  }
};

} // namespace THREAD_POOL_IMPL

template<typename FuncT, typename... ArgsT>
auto ThreadPool::ScheduleAndGetFuture(FuncT&& func, ArgsT&&... args)
    -> std::future<decltype(INVOKE_MACRO(func, ArgsT, args))>
{
  using ReturnT = decltype(INVOKE_MACRO(func, ArgsT, args));

  // We are only allocating this std::promise in a shared_ptr because
  // std::promise is non-copyable.
  std::shared_ptr<std::promise<ReturnT>> promise = std::make_shared<std::promise<ReturnT>>();
  std::future<ReturnT> retFuture = promise->get_future();

  THREAD_POOL_IMPL::FuncWrapper<ReturnT> funcWrapper{};
  std::function<void()> wrappedFunc = funcWrapper.GetWrapped(
      std::forward<FuncT>(func), std::move(promise), std::forward<ArgsT>(args)...);

  // Acquire the lock, and then push the WorkItem onto the queue.
  {
    const std::lock_guard<std::mutex> lock{m_mutex};
    WorkItem work{};
    work.func = std::move(wrappedFunc);
    m_workQueue.emplace(std::move(work));
  }
  m_newWorkCondition.notify_one(); // Tell one worker we are ready.

  return retFuture;
}

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif

#endif
