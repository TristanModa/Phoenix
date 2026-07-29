file(MAKE_DIRECTORY DEST_DIR)

# Glob all source files
file(GLOB_RECURSE SRC_FILES RELATIVE "${SRC_DIR}" "${SRC_DIR}/*")

# Glob existing dest files
file(GLOB_RECURSE DEST_FILES RELATIVE "${DEST_DIR}" "${DEST_DIR}/*")

# Remove any deleted files from dest
foreach (DEST_FILE ${DEST_FILES})
    list(FIND SRC_FILES "${DEST_FILE}" INDEX)
    if (INDEX EQUAL -1)
        message(STATUS "Removing deleted resource: ${DEST_FILE}")
        file(REMOVE "${DEST_DIR}/${DEST_FILE}")
    endif ()
endforeach ()

# Copy any new or modifed files
foreach (SRC_FILE ${SRC_FILES})
    set(SRC_PATH "${SRC_DIR}/${SRC_FILE}")
    set(DEST_PATH "${DEST_DIR}/${SRC_FILE}")

    # Check if the file needs to be copied
    set(NEED_COPY FALSE)
    set(NEW_RESOURCE FALSE)
    if (NOT EXISTS "${DEST_PATH}")
        set(NEED_COPY TRUE)
        set(NEW_RESOURCE TRUE)
    else ()
        file(TIMESTAMP "${SRC_PATH}" SRC_TIME "%Y-%m-%d-%H:%M:%S")
        file(TIMESTAMP "${DEST_PATH}" DEST_TIME "%Y-%m-%d-%H:%M:%S")
        if ("${SRC_TIME}" STRGREATER "${DEST_TIME}")
            set(NEED_COPY TRUE)
        endif ()
    endif ()

    # Copy the file if required
    if (NEED_COPY)
        if (NEW_RESOURCE)
            message(STATUS "Creating resource: ${SRC_FILE}")
        else ()
            message(STATUS "Updating resource: ${SRC_FILE}")
        endif ()
        get_filename_component(RESOURCE_DIR "${SRC_FILE}" DIRECTORY)
        file(COPY "${SRC_PATH}" DESTINATION "${DEST_DIR}/${RESOURCE_DIR}")
    endif ()
endforeach ()