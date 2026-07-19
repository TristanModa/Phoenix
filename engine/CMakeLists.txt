add_library(VulfEngine SHARED
        vulfEngine.h
        application.h
        application.c

		# Collections
		collections/collections.h
		collections/arrayList.h
		collections/arrayList.c
		collections/collectionsCommon.h
		collections/linkedList.h
		collections/linkedList.c

        # Core
        core/core.h
		core/input.h
		core/input.c
        core/logger.h
        core/logger.c
        core/memory.h
        core/memory.c
		core/resources.h
		core/resources.c
        core/types.h
        core/vtime.h
        core/vtime.c
        core/window.h
        core/window.c

		# GFX
		gfx/gfx.h
		gfx/colors.h
		gfx/renderer.h
		gfx/renderer.c
)

target_include_directories(VulfEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(VulfEngine PUBLIC
        vendor
)