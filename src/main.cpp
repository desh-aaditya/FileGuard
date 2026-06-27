#include <iostream>
#include <vector>
#include <string>
#include <csignal>
#include <thread>
#include <chrono>
#include "watcher.h"
#include "logger.h"

Watcher* g_watcher = nullptr;

void signal_handler(int) {
    std::cout << "\n[FileGuard] Stopping...\n";
    if (g_watcher) g_watcher->stop();
}

void print_usage() {
    std::cout << "Usage: fileguard <directory> [extensions...]\n";
    std::cout << "Example: fileguard C:\\Users\\You\\Documents .cpp .h\n";
    std::cout << "         fileguard C:\\Users\\You\\Documents\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string watch_dir = argv[1];

    // Collect extension filters from CLI args (STL vector)
    std::vector<std::string> extensions;
    for (int i = 2; i < argc; ++i)
        extensions.emplace_back(argv[i]);

    std::signal(SIGINT, signal_handler);

    std::cout << "FileGuard v1.0 - Real-time File Watcher\n";
    std::cout << "========================================\n";

    try {
        Logger logger("fileguard.log");
        Watcher watcher(watch_dir, extensions, logger);

        g_watcher = &watcher;
        watcher.start();

        std::cout << "Press Ctrl+C to stop.\n";
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }

    return 0;
}