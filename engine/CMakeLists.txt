add_library(VulfEngine SHARED
        vulfEngine.h
        application.h
        application.c
        core/types.h
        core/core.h
        core/window.h
        core/memory.h
        core/memory.c
        core/window.c
)

target_include_directories(VulfEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VulfEngine PUBLIC
        vendor
)