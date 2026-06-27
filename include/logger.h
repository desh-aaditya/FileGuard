#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <ctime>

class Logger {
public:
    explicit Logger(const std::string& logfile = "fileguard.log") {
        log_stream_.open(logfile, std::ios::app);
        if (!log_stream_.is_open())
            std::cerr << "[Logger] Failed to open log file: " << logfile << "\n";
    }

    // RAII: destructor closes file automatically
    ~Logger() {
        if (log_stream_.is_open())
            log_stream_.close();
    }

    void log(const std::string& event, const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex_);  // thread-safe
        std::string entry = timestamp() + " [" + event + "] " + path;
        std::cout << entry << "\n";
        if (log_stream_.is_open())
            log_stream_ << entry << "\n";
    }

private:
    std::ofstream log_stream_;
    std::mutex mutex_;

    std::string timestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        return std::string(buf);
    }
};