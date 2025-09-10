cmake_minimum_required(VERSION 3.16)
project(micro80emu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(micro80emu
        main.cpp
        I8080.cpp
)
