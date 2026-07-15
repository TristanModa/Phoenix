add_library(VulfEngine SHARED
        vulfEngine.h
        application.h
        application.c

        # Core
        core/core.h
        core/logger.h
        core/logger.c
        core/memory.h
        core/memory.c
        core/types.h
        core/vtime.h
        core/vtime.c
        core/window.h
        core/window.c
)

target_include_directories(VulfEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VulfEngine PUBLIC
        vendor
)