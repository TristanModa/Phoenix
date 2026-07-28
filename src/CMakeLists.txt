# Create the executable
add_executable(${PROJECT_NAME}
        # Root
		main.c
        application.h
        application.c

		# Engine
		engine/input.h
		engine/input.c
		engine/logger.c
		engine/logger.h
		engine/types.h
		engine/vtime.h
		engine/vtime.c
		engine/window.h
		engine/window.c

		# Engine::Collections
		engine/collections/collectionsCommon.h
		engine/collections/arrayList.h
		engine/collections/arrayList.c
		engine/collections/hashMap.h
		engine/collections/hashMap.c
)

# Configure the executable
target_compile_definitions(${PROJECT_NAME} PRIVATE
        VERSION_MAJOR="${PROJECT_VERSION_MAJOR}"
        VERSION_MINOR="${PROJECT_VERSION_MINOR}"
        VERSION_PATCH="${PROJECT_VERSION_PATCH}"
)
target_link_libraries(${PROJECT_NAME} PRIVATE vendor)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")