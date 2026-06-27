cmake_minimum_required(VERSION 3.15)
project(FileGuard VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(fileguard
    src/main.cpp
    src/watcher.cpp
)

target_link_libraries(fileguard winpthread)