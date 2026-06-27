# FileGuard
> A multithreaded, real-time CLI file watcher built in C++17 for Windows.

---

## Overview

FileGuard monitors a directory for file system changes — creations, modifications, deletions, and renames — and logs them in real-time to the console and a log file.

Built using **Windows API (`ReadDirectoryChangesW`)**, **C++17 STL**, **RAII**, and **std::thread** for background processing.

---

## Features

- Real-time file event detection (create, modify, delete, rename)
- Optional extension-based filtering (e.g. only `.cpp`, `.h` files)
- Multithreaded — watcher runs on a background thread
- Logs all events to console + `fileguard.log` file
- RAII-managed Windows HANDLE for safe resource cleanup
- Built with modern C++17 (STL, atomic flags, chrono)

---

## Tech Stack

| Component | Technology |
|-----------|-----------|
| Language | C++17 |
| File Watching | Windows API (`ReadDirectoryChangesW`) |
| Concurrency | `std::thread`, `std::atomic`, `std::mutex` |
| Resource Management | RAII (`CloseHandle` in destructor) |
| Filtering | STL (`std::vector`, `std::any_of`) |
| Build System | CMake 3.15+ |
| Compiler | MinGW-w64 (GCC 16.1.0) |

---

## Project Structure

```
FileGuard/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── logger.h       # Thread-safe logger with RAII file handling
│   └── watcher.h      # Watcher class declaration
└── src/
    ├── main.cpp       # Entry point, CLI argument parsing, signal handling
    └── watcher.cpp    # Core file watching logic using Windows API
```

---

## Prerequisites

- Windows 10/11
- [CMake 3.15+](https://cmake.org/download/)
- [MinGW-w64](https://github.com/niXman/mingw-builds-binaries/releases) (GCC 16.x, posix-seh-ucrt)

---

## Build Instructions

```bash
# Clone the repo
git clone https://github.com/YOUR_USERNAME/FileGuard.git
cd FileGuard

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -G "MinGW Makefiles"

# Build
cmake --build .
```

The binary `fileguard.exe` will be created inside the `build/` directory.

---

## Usage

```bash
# Watch all files in a directory
fileguard.exe <directory>

# Watch only specific file extensions
fileguard.exe <directory> .cpp .h .txt
```

### Examples

```bash
# Watch your src folder for any changes
.\fileguard.exe D:\MyProject\src

# Watch Documents folder for .txt and .pdf files only
.\fileguard.exe C:\Users\YourName\Documents .txt .pdf
```

---

## Sample Output

```
FileGuard v1.0 - Real-time File Watcher
========================================
2026-06-28 01:05:06 [STARTED]      Watching: D:\FileGuard\src
Press Ctrl+C to stop.
2026-06-28 01:05:20 [CREATED]      D:\FileGuard\src\test.cpp
2026-06-28 01:05:20 [MODIFIED]     D:\FileGuard\src\test.cpp
2026-06-28 01:05:45 [DELETED]      D:\FileGuard\src\test.cpp
2026-06-28 01:06:10 [RENAMED FROM] D:\FileGuard\src\old_name.cpp
2026-06-28 01:06:10 [RENAMED TO]   D:\FileGuard\src\new_name.cpp
```

> **Note:** A double `MODIFIED` event on file creation is normal Windows behavior — Windows fires one event for content and one for metadata.

All events are also saved to `fileguard.log` in the directory where you run the executable.

---

## Key Concepts Demonstrated

- **RAII** — Windows `HANDLE` opened in constructor, closed in destructor automatically
- **Multithreading** — `std::thread` runs the watch loop in background; `std::atomic<bool>` flag for safe shutdown
- **Thread Safety** — `std::mutex` + `std::lock_guard` in Logger prevents race conditions
- **STL** — `std::vector` for extensions, `std::any_of` for filtering, `std::string` for path handling
- **Modern C++17** — `std::filesystem`, structured bindings, `if constexpr`
- **Windows API** — `ReadDirectoryChangesW`, `CreateFileA`, `CloseHandle`

---

## How It Works

```
main.cpp
  │
  ├── parses CLI args (directory + extensions)
  ├── creates Logger (opens log file via RAII)
  ├── creates Watcher
  └── calls watcher.start()
          │
          └── spawns std::thread → watch_loop()
                    │
                    └── ReadDirectoryChangesW() [blocks until event]
                              │
                              ├── parse FILE_NOTIFY_INFORMATION buffer
                              ├── apply extension filter (STL)
                              └── logger.log(event, path)
```

---

## Stopping

Press `Ctrl+C` — the signal handler sets the atomic flag to `false`, the background thread exits cleanly, and the HANDLE is closed via RAII destructor.

---

## License

MIT License — free to use, modify, and distribute.

---

## Author

**Aaditya Deshpande**
- GitHub: [@desh-aaditya](https://github.com/desh-aaditya)
- LinkedIn: [aaditya-deshpande165](https://www.linkedin.com/in/aaditya-deshpande165/)