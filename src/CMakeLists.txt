# Create the executable
add_executable(${PROJECT_NAME}
        # Root
		main.c
        application.h
        application.c

		# Engine::Collections
		engine/collections/collectionsCommon.h
		engine/collections/arrayList.h
		engine/collections/arrayList.c
		engine/collections/hashMap.h
		engine/collections/hashMap.c

		# Engine::Core
		engine/core/input.h
		engine/core/input.c
		engine/core/logger.c
		engine/core/logger.h
		engine/core/types.h
		engine/core/resources.h
		engine/core/resources.c
		engine/core/vtime.h
		engine/core/vtime.c
		engine/core/window.h
		engine/core/window.c

		# Engine::Debug
		engine/debug/debugPanel.h
		engine/debug/debugPanel.c

		# Engine::Rendering
		engine/rendering/colors.h
		engine/rendering/renderer.h
		engine/rendering/renderer.c

		# Phoenix
		phoenix/phoenix.h
		phoenix/phoenix.c
)

# Configure the executable
target_compile_definitions(${PROJECT_NAME} PRIVATE
        VERSION_MAJOR="${PROJECT_VERSION_MAJOR}"
        VERSION_MINOR="${PROJECT_VERSION_MINOR}"
        VERSION_PATCH="${PROJECT_VERSION_PATCH}"
		$<$<CONFIG:Debug>:DEBUG>
)
target_link_libraries(${PROJECT_NAME} PRIVATE vendor)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
add_dependencies(${PROJECT_NAME} SyncResources)