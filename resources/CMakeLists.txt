include(FetchContent)

# Get ImGui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY	https://github.com/ocornut/imgui.git
        GIT_TAG			v1.92.9
)
FetchContent_MakeAvailable(imgui)

# Add Bin2H utility
add_executable(Bin2H "${imgui_SOURCE_DIR}/misc/fonts/binary_to_compressed_c.cpp")
set_target_properties(Bin2H PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

# Glob all resources
set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
file(GLOB_RECURSE RESOURCE_FILES CONFIGURE_DEPENDS RELATIVE "${RESOURCES_DIR}" "${RESOURCES_DIR}/*")

# Generate resources headers
set(DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_HEADERS "")
foreach (RESOURCE ${RESOURCE_FILES})
    get_filename_component(FILE_SUBPATH ${RESOURCE} DIRECTORY)
    get_filename_component(FILE_NAME ${RESOURCE} NAME_WE)
    string(REPLACE "/" "_" GENERATED_FILE_NAME "${FILE_SUBPATH}/${FILE_NAME}")
    set(GENERATED_FILE "${DEST_DIR}/data/${GENERATED_FILE_NAME}.h")
    add_custom_command(
            OUTPUT "${GENERATED_FILE}"
            COMMAND Bin2H -nocompress "${RESOURCES_DIR}/${RESOURCE}" "${GENERATED_FILE_NAME}" > "${GENERATED_FILE}"
            DEPENDS Bin2H "${RESOURCES_DIR}/${RESOURCE}"
            VERBATIM
    )

    list(APPEND GENERATED_HEADERS ${GENERATED_FILE})
endforeach ()

# Create a target for building resources
add_custom_target(BuildResources ALL DEPENDS ${GENERATED_HEADERS})

# Generate a header containing an enum of all resources and a get function definition
set(RESOURCE_DATA_HEADER "${DEST_DIR}/resourceData.h")
file(WRITE "${RESOURCE_DATA_HEADER}" "#ifndef GENERATED_RESOURCES_DATA_H\n#define GENERATED_RESOURCES_DATA_H\n\ntypedef enum resourceID {\n")
foreach (HEADER ${GENERATED_HEADERS})
    get_filename_component(FILE_NAME ${HEADER} NAME_WE)
    string(REGEX REPLACE "_" "__" PATH_FIX "${FILE_NAME}")
    string(REGEX REPLACE "([a-z])([A-Z])" "\\1_\\2" CAMEL_SPLIT "${PATH_FIX}")
    string(TOUPPER "${CAMEL_SPLIT}" ENUM_NAME)
    file(APPEND "${RESOURCE_DATA_HEADER}" "\t${ENUM_NAME},\n")
    set_source_files_properties("${HEADER}" PROPERTIES CACHED_ENUM_NAME "${ENUM_NAME}")
endforeach ()
file(APPEND "${RESOURCE_DATA_HEADER}" "} ResourceID;\n\nunsigned char* getResourceData(ResourceID resourceID, unsigned int* size);\n\n#endif")

# Generate a source for the definition of the get function
set(RESOURCE_DATA_SOURCE "${DEST_DIR}/resourceData.c")
file(WRITE "${RESOURCE_DATA_SOURCE}" "#include \"resourceData.h\"\n\n")
foreach (HEADER ${GENERATED_HEADERS})
    get_filename_component(FILE_NAME ${HEADER} NAME)
    file(APPEND "${RESOURCE_DATA_SOURCE}" "#include \"data/${FILE_NAME}\"\n")
endforeach ()
file(APPEND "${RESOURCE_DATA_SOURCE}" "\nunsigned char* getResourceData(ResourceID resourceID, unsigned int* size) {\n\tswitch (resourceID) {\n")
foreach (HEADER ${GENERATED_HEADERS})
    get_source_file_property(ENUM_NAME "${HEADER}" CACHED_ENUM_NAME)
    get_filename_component(FILE_NAME "${HEADER}" NAME_WE)
    file(APPEND "${RESOURCE_DATA_SOURCE}" "\t\tcase ${ENUM_NAME}:\n\t\t\t*size = ${FILE_NAME}_size;\n\t\t\treturn (void*)${FILE_NAME}_data;\n")
endforeach ()
file (APPEND "${RESOURCE_DATA_SOURCE}" "\t\tdefault: \n\t\t\t*size = 0;\n\t\t\treturn nullptr;\n\t}\n}")

# Add the resources interface
add_library(Resources STATIC "${RESOURCE_DATA_SOURCE}")
target_include_directories(Resources PUBLIC "${DEST_DIR}")
add_dependencies(Resources BuildResources)