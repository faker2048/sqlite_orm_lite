#pragma once

#include <functional>
#include <iostream>
#include <memory>

namespace sqliteol {

class Logger {
 public:
  struct LogFunction {
    std::function<void(const std::string&)> trace    = nullptr;
    std::function<void(const std::string&)> debug    = nullptr;
    std::function<void(const std::string&)> info     = nullptr;
    std::function<void(const std::string&)> error    = nullptr;
    std::function<void(const std::string&)> critical = nullptr;
  };

  inline static Logger& getInstance() {
    static Logger instance;
    return instance;
  }

  Logger(const Logger&)            = delete;
  Logger& operator=(const Logger&) = delete;

  inline void SetLogFunctions(const LogFunction& log_functions) {
    log_functions_ = log_functions;
  }

  inline void trace(const std::string& message) {
    log_functions_.trace(message);
  }

  inline void debug(const std::string& message) {
    log_functions_.debug(message);
  }

  inline void info(const std::string& message) {
    log_functions_.info(message);
  }

  inline void error(const std::string& message) {
    log_functions_.error(message);
  }

  inline void critical(const std::string& message) {
    log_functions_.critical(message);
  }

 private:
  inline Logger() {
    log_functions_.trace = [](const std::string& message) {
      std::cout << "TRACE: " << message << std::endl;
    };

    log_functions_.debug = [](const std::string& message) {
      std::cout << "DEBUG: " << message << std::endl;
    };

    log_functions_.info = [](const std::string& message) {
      std::cout << "INFO: " << message << std::endl;
    };

    log_functions_.error = [](const std::string& message) {
      std::cout << "ERROR: " << message << std::endl;
    };

    log_functions_.critical = [](const std::string& message) {
      std::cout << "CRITICAL: " << message << std::endl;
    };
  }

  LogFunction log_functions_;
};

}  // namespace sqliteol