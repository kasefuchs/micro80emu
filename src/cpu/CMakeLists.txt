add_library(cpu
    I8080.cpp
)

target_include_directories(cpu
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cpu
    PRIVATE common
)
