#pragma once

#include <string>
#include <string_view>
#include <format>
#include <iostream>
#include <mutex>
#include <source_location>

/** @brief Log severity levels for output filtering. */
enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal
};

/** @brief Thread-safe, ANSI-colored console logger featuring C++20 source location tracking. */
class Logger {
public:
    /**
     * @brief Enables ANSI escape sequence color support in Windows console (no-op on Linux/macOS).
     * @note Should be called once during engine initialization.
     */
    static void init();

    /**
     * @brief Sets the minimum severity level to display in the console.
     * @param level Minimum active log level.
     */
    static void setLogLevel(const LogLevel level) { m_minLevel = level; }

    /**
     * @brief Core logging method supporting variadic template formatting and location capture.
     *
     * @tparam Args Format argument types.
     * @param level Severity level of the message.
     * @param location Caller source location (auto-filled via macro).
     * @param fmtFormat Format string (`std::format` syntax).
     * @param args Arguments to inject into the format string.
     */
    template<typename... Args>
    static void log(const LogLevel level, const std::source_location location,
        const std::string_view fmtFormat, Args&&... args) {
        if (level < m_minLevel) return;

        std::string formattedMsg;
        try {
            formattedMsg = std::vformat(fmtFormat, std::make_format_args(args...));
        } catch (const std::exception& e) {
            formattedMsg = std::string("Formatting Error: ") + e.what();
        }

        writeLog(level, location, formattedMsg);
    }

private:
    static void writeLog(LogLevel level, const std::source_location& loc, const std::string& message);

    static LogLevel m_minLevel; ///< Current minimum visible log level threshold.
    static std::mutex m_mutex;  ///< Mutex enforcing thread-safe console output.

    static bool s_initialized;  ///< Is the logger already initialized?
};

// Global convenience logging macros with automatic source location injection
#define LOG_TRACE(...) Logger::log(LogLevel::Trace, std::source_location::current(), __VA_ARGS__)
#define LOG_DEBUG(...) Logger::log(LogLevel::Debug, std::source_location::current(), __VA_ARGS__)
#define LOG_INFO(...)  Logger::log(LogLevel::Info,  std::source_location::current(), __VA_ARGS__)
#define LOG_WARN(...)  Logger::log(LogLevel::Warn,  std::source_location::current(), __VA_ARGS__)
#define LOG_ERROR(...) Logger::log(LogLevel::Error, std::source_location::current(), __VA_ARGS__)
#define LOG_FATAL(...) Logger::log(LogLevel::Fatal, std::source_location::current(), __VA_ARGS__)