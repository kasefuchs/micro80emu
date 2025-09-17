add_executable(
        ${PROJECT_NAME}
        main.cpp
        I8080.cpp
        Screen.cpp
        Hardware.cpp
)

find_package(cxxopts REQUIRED)
find_package(raylib REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CXXOPTS_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
