cmake_minimum_required(VERSION 3.16)
project(micro80emu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(RAYLIB_VERSION 5.5)
set(CXXOPTS_VERSION v3.3.1)

# Raylib
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG ${RAYLIB_VERSION}
)
FetchContent_MakeAvailable(raylib)

# cxxopts
FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG ${CXXOPTS_VERSION}
)
FetchContent_MakeAvailable(cxxopts)

add_subdirectory(src)
