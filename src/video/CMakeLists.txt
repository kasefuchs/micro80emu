add_library(video
    Video.cpp
)

target_include_directories(video
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(video
    PRIVATE common
    PRIVATE raylib
)
