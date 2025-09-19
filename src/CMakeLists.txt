add_executable(
    ${PROJECT_NAME}
    main.cpp
    I8080.cpp
    Screen.cpp
    Keyboard.cpp
    Hardware.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE cxxopts::cxxopts raylib)
