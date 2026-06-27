#include "watcher.h"
#include <windows.h>
#include <stdexcept>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

Watcher::Watcher(const std::string& path,
                 const std::vector<std::string>& extensions,
                 Logger& logger)
    : watch_path_(path), extensions_(extensions),
      logger_(logger), running_(false)
{
    if (!fs::exists(path))
        throw std::runtime_error("Path does not exist: " + path);
}

Watcher::~Watcher() {
    stop();
}

void Watcher::start() {
    running_ = true;
    worker_ = std::thread(&Watcher::watch_loop, this);
    logger_.log("STARTED", "Watching: " + watch_path_);
}

void Watcher::stop() {
    running_ = false;
    if (worker_.joinable())
        worker_.join();
}

void Watcher::watch_loop() {
    // Open directory handle (RAII-style via Windows API)
    HANDLE hDir = CreateFileA(
        watch_path_.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        logger_.log("ERROR", "Could not open directory handle");
        return;
    }

    // Buffer for file change events
    char buf[4096];
    DWORD bytes_returned;

    while (running_) {
        // Windows equivalent of inotify — blocks until change detected
        BOOL result = ReadDirectoryChangesW(
            hDir,
            buf,
            sizeof(buf),
            FALSE,  // not watching subdirectories
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_LAST_WRITE |
            FILE_NOTIFY_CHANGE_SIZE,
            &bytes_returned,
            NULL,
            NULL
        );

        if (!result || !running_) break;

        // Parse events from buffer (STL usage)
        FILE_NOTIFY_INFORMATION* event =
            reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buf);

        do {
            // Convert wide string filename to regular string
            int len = WideCharToMultiByte(CP_UTF8, 0,
                event->FileName,
                event->FileNameLength / sizeof(WCHAR),
                nullptr, 0, nullptr, nullptr);

            std::string filename(len, '\0');
            WideCharToMultiByte(CP_UTF8, 0,
                event->FileName,
                event->FileNameLength / sizeof(WCHAR),
                &filename[0], len, nullptr, nullptr);

            std::string full_path = watch_path_ + "\\" + filename;

            if (extensions_.empty() || matches_filter(filename)) {
                switch (event->Action) {
                    case FILE_ACTION_ADDED:
                        logger_.log("CREATED", full_path); break;
                    case FILE_ACTION_MODIFIED:
                        logger_.log("MODIFIED", full_path); break;
                    case FILE_ACTION_REMOVED:
                        logger_.log("DELETED", full_path); break;
                    case FILE_ACTION_RENAMED_OLD_NAME:
                        logger_.log("RENAMED FROM", full_path); break;
                    case FILE_ACTION_RENAMED_NEW_NAME:
                        logger_.log("RENAMED TO", full_path); break;
                }
            }

            if (event->NextEntryOffset == 0) break;
            event = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                reinterpret_cast<char*>(event) + event->NextEntryOffset);

        } while (true);
    }

    CloseHandle(hDir);  // RAII cleanup
}

bool Watcher::matches_filter(const std::string& filename) {
    // STL algorithm
    return std::any_of(extensions_.begin(), extensions_.end(),
        [&filename](const std::string& ext) {
            return filename.size() >= ext.size() &&
                   filename.substr(filename.size() - ext.size()) == ext;
        });
}