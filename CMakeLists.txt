cmake_minimum_required(VERSION 3.16)
project(micro80emu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
        ${PROJECT_NAME}
        main.cpp
        I8080.cpp
)

find_package(cxxopts QUIET)

target_include_directories(${PROJECT_NAME} PRIVATE ${CXXOPTS_INCLUDE_DIRS})
