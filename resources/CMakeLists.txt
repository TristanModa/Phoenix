# Create target for syncing resources to the build directory
set(RESOURCES_SRC "${CMAKE_CURRENT_SOURCE_DIR}/resources")
set(RESOURCES_DEST "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources")
add_custom_target(SyncResources ALL
        COMMAND ${CMAKE_COMMAND}
        -DSRC_DIR="${RESOURCES_SRC}"
        -DDEST_DIR="${RESOURCES_DEST}"
        -DSHADER_SRC="${CMAKE_CURRENT_SOURCE_DIR}/shaders-src"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/SyncResources.cmake"
        COMMENT "Syncing resources..."
)