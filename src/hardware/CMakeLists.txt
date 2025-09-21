add_library(hardware
    Keyboard.cpp
    Hardware.cpp
)

target_include_directories(hardware
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hardware
    PRIVATE common
    PRIVATE cpu
    PRIVATE video
    PRIVATE raylib
)
