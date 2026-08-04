#include "Logger.h"
#include <chrono>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#endif

LogLevel Logger::m_minLevel = LogLevel::Trace;
std::mutex Logger::m_mutex;

void Logger::init() {
#ifdef _WIN32
    // Enable VT100 ANSI escape processing mode in Windows terminal
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
#endif
}

void Logger::writeLog(const LogLevel level, const std::source_location& loc, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Format timestamp [HH:MM:SS]
    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tmBuffer{};
#ifdef _WIN32
    localtime_s(&tmBuffer, &now);
#else
    localtime_r(&now, &tmBuffer);
#endif

    // Resolve color codes and severity string
    auto colorCode = "";
    auto levelStr  = "";

    switch (level) {
        case LogLevel::Trace: colorCode = "\033[90m"; levelStr = "TRACE"; break; // Dark Gray
        case LogLevel::Debug: colorCode = "\033[36m"; levelStr = "DEBUG"; break; // Cyan
        case LogLevel::Info:  colorCode = "\033[32m"; levelStr = "INFO "; break; // Green
        case LogLevel::Warn:  colorCode = "\033[33m"; levelStr = "WARN "; break; // Yellow
        case LogLevel::Error: colorCode = "\033[31m"; levelStr = "ERROR"; break; // Red
        case LogLevel::Fatal: colorCode = "\033[35m"; levelStr = "FATAL"; break; // Magenta
    }

    const auto resetCode = "\033[0m";

    // Extract pure file name from full absolute file path
    std::string_view filePath = loc.file_name();
    if (const auto lastSlash = filePath.find_last_of("/\\"); lastSlash != std::string_view::npos) {
        filePath.remove_prefix(lastSlash + 1);
    }

    // Output unified console string
    std::cout << colorCode
              << std::format("[{:02d}:{:02d}:{:02d}] [{}] [{}:{}] {}",
                             tmBuffer.tm_hour, tmBuffer.tm_min, tmBuffer.tm_sec,
                             levelStr, filePath, loc.line(), message)
              << resetCode << "\n";
}