#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include "logger.h"

class Watcher {
public:
    Watcher(const std::string& path,
            const std::vector<std::string>& extensions,
            Logger& logger);

    ~Watcher();

    void start();
    void stop();

private:
    void watch_loop();
    bool matches_filter(const std::string& filename);

    std::string watch_path_;
    std::vector<std::string> extensions_;
    Logger& logger_;

    std::thread worker_;
    std::atomic<bool> running_;
};