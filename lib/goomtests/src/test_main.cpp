#define CATCH_CONFIG_RUNNER
//#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file

#undef NO_LOGGING
#include "catch2/catch.hpp"
#include "logging.h"

#include <iostream>
#include <ostream>
#include <string>

using GOOM::UTILS::Logging;

auto main(int argc, char* argv[]) -> int
{
  // global setup...
  const auto f_console_log = [](Logging::LogLevel, const std::string& s) {
    std::clog << s << std::endl;
  };
  AddLogHandler("console-log", f_console_log);
  SetLogLevel(Logging::LogLevel::INFO);
  SetLogLevelForFiles(Logging::LogLevel::INFO);
  LogStart();
  LogInfo("Start unit tests...");

  int result = Catch::Session().run(argc, argv);

  // global clean-up...

  return result;
}
