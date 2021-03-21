#ifndef LIB_GOOMUTILS_INCLUDE_GOOMUTILS_LOGGING_H_
#define LIB_GOOMUTILS_INCLUDE_GOOMUTILS_LOGGING_H_

#include <format>
#include <fstream>
#include <functional>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
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

class Logging
{
public:
  enum class LogLevel
  {
    DEBUG,
    INFO,
    WARN,
    ERROR
  };
  using HandlerFunc = std::function<void(const LogLevel, const std::string&)>;

  static auto GetLogger() -> Logging&;
  ~Logging();
  Logging(const Logging&) = delete;
  Logging(Logging&&) = delete;
  auto operator=(const Logging&) -> Logging& = delete;
  auto operator=(Logging&&) -> Logging& = delete;

  void SetLogFile(const std::string& logF);
  void AddHandler(const std::string& name, const HandlerFunc& f);
  void Start();
  void Stop();
  void Flush();
  void Suspend();
  void Resume();
  [[nodiscard]] auto IsLogging() const -> bool;
  [[nodiscard]] auto GetFileLogLevel() const -> LogLevel;
  void SetFileLogLevel(LogLevel lvl);
  [[nodiscard]] auto GetHandlersLogLevel() const -> LogLevel;
  void SetHandlersLogLevel(LogLevel lvl);
  void Log(LogLevel lvl, int line_num, const std::string& func_name, const std::string& msg);
  template<typename... Args>
  void Log(LogLevel lvl,
           int line_num,
           const std::string& func_name,
           const std::string& format_str,
           const Args&... args);

private:
  Logging() noexcept;
  LogLevel m_cutoffFileLogLevel = LogLevel::INFO;
  LogLevel m_cutoffHandlersLogLevel = LogLevel::INFO;
  bool m_doLogging = false;
  std::string m_logFile{};
  std::vector<std::pair<std::string, HandlerFunc>> m_handlers{};
  std::vector<std::string> m_logEntries{};
  std::mutex m_mutex{};
  static std::unique_ptr<Logging> logger;
  void DoFlush();
  void VLog(LogLevel lvl,
            int line_num,
            const std::string& func_name,
            const std::string& format_str,
            std20::format_args args);
};

inline auto Logging::GetFileLogLevel() const -> Logging::LogLevel
{
  return m_cutoffFileLogLevel;
}

inline auto Logging::GetHandlersLogLevel() const -> Logging::LogLevel
{
  return m_cutoffHandlersLogLevel;
}

inline void Logging::SetLogFile(const std::string& logF)
{
  m_logFile = logF;
}

inline void Logging::Suspend()
{
  m_doLogging = false;
}
inline void Logging::Resume()
{
  m_doLogging = true;
}
inline auto Logging::IsLogging() const -> bool
{
  return m_doLogging;
}
inline auto Logging::GetLogger() -> Logging&
{
  return *logger;
}

template<typename... Args>
inline void Logging::Log(LogLevel lvl,
                         int line_num,
                         const std::string& func_name,
                         const std::string& format_str,
                         const Args&... args)
{
  VLog(lvl, line_num, func_name, format_str, std20::make_format_args(args...));
}

inline void Logging::VLog(LogLevel lvl,
                          int line_num,
                          const std::string& func_name,
                          const std::string& format_str,
                          std20::format_args args)
{
  std20::memory_buffer buffer;
  // Pass custom argument formatter as a template arg to vwrite.
  std20::vformat_to(std20::detail::buffer_appender<char>(buffer), format_str, args);
  Log(lvl, line_num, func_name, std::string(buffer.data(), buffer.size()));
}

#ifdef NO_LOGGING
#pragma message("Compiling " __FILE__ " with 'NO_LOGGING' ON.")
#define SetLogFile(logF)
#define AddLogHandler(name, h)
#define LogStart()
#define LogStop()
#define LogFlush()
#define LogSuspend()
#define LogResume()
#define IsLogging() false
#define GetLogLevel()
#define SetLogLevel(lvl)
#define LogDebug(...)
#define LogInfo(...)
#define LogWarn(...)
#define LogError(...)
#else
#pragma message("Compiling " __FILE__ " with 'NO_LOGGING' OFF.")
#define SetLogFile(logF) Logging::GetLogger().SetLogFile(logF)
#define AddLogHandler(name, h) Logging::GetLogger().AddHandler(name, h);
#define LogStart() Logging::GetLogger().Start()
#define LogStop() Logging::GetLogger().Stop()
#define LogFlush() Logging::GetLogger().Flush()
#define lLogSuspend() Logging::GetLogger().Suspend()
#define LogResume() Logging::GetLogger().Resume()
#define IsLogging() Logging::GetLogger().IsLogging()
#define GetLogLevel() Logging::GetLogger().GetHandlersLogLevel()
#define SetLogLevel(lvl) Logging::GetLogger().SetHandlersLogLevel(lvl)
#define GetLogLevelForFiles() Logging::GetLogger().GetFileLogLevel()
#define SetLogLevelForFiles(lvl) Logging::GetLogger().SetFileLogLevel(lvl)
#define LogDebug(...) \
  Logging::GetLogger().Log(Logging::LogLevel::DEBUG, __LINE__, __func__, __VA_ARGS__)
#define LogInfo(...) \
  Logging::GetLogger().Log(Logging::LogLevel::INFO, __LINE__, __func__, __VA_ARGS__)
#define LogWarn(...) \
  Logging::GetLogger().Log(Logging::LogLevel::WARN, __LINE__, __func__, __VA_ARGS__)
#define LogError(...) \
  Logging::GetLogger().Log(Logging::LogLevel::ERROR, __LINE__, __func__, __VA_ARGS__)
#endif


#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
#endif
