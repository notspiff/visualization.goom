#include "goomutils/logging.h"

#include <functional>
#include <memory>
#include <mutex>
#include <ostream>
#include <stdexcept>
#include <string>

#if __cplusplus <= 201402L
namespace GOOM
{
namespace UTILS
{
#else
namespace GOOM::UTILS
{
#endif

std::unique_ptr<Logging> Logging::logger(new Logging());

Logging::Logging() noexcept
{
  SetFileLogLevel(m_cutoffFileLogLevel);
  SetHandlersLogLevel(m_cutoffHandlersLogLevel);
}

Logging::~Logging()
{
  DoFlush();
}

void Logging::Log(LogLevel lvl, int line_num, const std::string& func_name, const std::string& msg)
{
  const std::lock_guard<std::mutex> lock{m_mutex};
  if (!m_doLogging)
  {
    return;
  }

  const std::string logMsg = func_name + ":" + std::to_string(line_num) + ":" + msg;

  if (lvl >= m_cutoffFileLogLevel)
  {
    m_logEntries.push_back(logMsg);
  }
  if (lvl >= m_cutoffHandlersLogLevel)
  {
#if __cplusplus <= 201402L
    for (const auto& handlerInfo : m_handlers)
    {
      const auto& handler = std::get<1>(handlerInfo);
#else
    for (const auto& [name, handler] : m_handlers)
    {
#endif
      handler(lvl, logMsg);
    }
  }
}

void Logging::Start()
{
  const std::lock_guard<std::mutex> lock{m_mutex};
  m_doLogging = true;
  m_logEntries.clear();
}

void Logging::Stop()
{
  const std::lock_guard<std::mutex> lock{m_mutex};
  m_doLogging = false;
  DoFlush();
}

void Logging::SetFileLogLevel(LogLevel lvl)
{
  m_cutoffFileLogLevel = lvl;
}

void Logging::SetHandlersLogLevel(LogLevel lvl)
{
  m_cutoffHandlersLogLevel = lvl;
}

void Logging::AddHandler(const std::string& name, const HandlerFunc& f)
{
#if __cplusplus <= 201402L
  for (const auto& handlerInfo : m_handlers)
  {
    const auto& hname = std::get<0>(handlerInfo);
#else
  for (const auto& [hname, handlr] : handlers)
  {
#endif
    if (hname == name)
    {
      return;
    }
  }
  m_handlers.emplace_back(name, f);
}

void Logging::Flush()
{
  const std::lock_guard<std::mutex> lock{m_mutex};
  DoFlush();
}

void Logging::DoFlush()
{
  if (m_logFile.empty())
  {
    return;
  }
  if (m_logEntries.empty())
  {
    return;
  }

  std::ofstream f(m_logFile, std::ios::out | std::ios::app);
  if (!f.good())
  {
    throw std::runtime_error("Could not open log file \"" + m_logFile + "\".");
  }
  for (const auto& entry : m_logEntries)
  {
    f << entry << std::endl;
  }

  m_logEntries.clear();
}

#if __cplusplus <= 201402L
} // namespace UTILS
} // namespace GOOM
#else
} // namespace GOOM::UTILS
#endif
