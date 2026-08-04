include(FetchContent)

# Get DXC
if (WIN32)
    find_package(Vulkan REQUIRED COMPONENTS dxc)
    set(DXC_EXECUTABLE "${Vulkan_dxc_EXECUTABLE}")
else ()
    # Get latest Vulkan
    FetchContent_Declare(
            vulkan
            URL         "https://sdk.lunarg.com/sdk/download/latest/linux/vulkan_sdk.tar.xz"
    )
    FetchContent_MakeAvailable(vulkan)
    set(DXC_EXECUTABLE "${vulkan_SOURCE_DIR}/x86_64/bin/dxc")
endif ()

# Glob all shader sources
set(SHADER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shader-src")
file(GLOB_RECURSE SHADER_SOURCE_FILES CONFIGURE_DEPENDS RELATIVE "${SHADER_SRC_DIR}" "${SHADER_SRC_DIR}/*.hlsl")

# Compile shaders
set(SHADER_DEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders")
set(COMPILED_SHADERS "")
foreach (SHADER ${SHADER_SOURCE_FILES})
    if (SHADER MATCHES "\\.vert")
        set(SHADER_TYPE "vs")
        set(SHADER_SUFFIX "Vert")
    elseif (SHADER MATCHES "\\.frag")
        set(SHADER_TYPE "ps")
        set(SHADER_SUFFIX "Frag")
    else ()
        message(WARNING "Invalid shader type: ${SHADER}")
        continue()
    endif ()

    get_filename_component(FILE_NAME ${SHADER} NAME_WE)
    string(REPLACE "." "_" FILE_NAME ${FILE_NAME})
    set(COMPILED_SHADER "${SHADER_DEST_DIR}/${FILE_NAME}${SHADER_SUFFIX}.spv")

    add_custom_command(
            OUTPUT "${COMPILED_SHADER}"
            COMMAND "${DXC_EXECUTABLE}" -spirv -T "${SHADER_TYPE}_6_0" -E main "${SHADER_SRC_DIR}/${SHADER}" -Fo "${COMPILED_SHADER}"
            DEPENDS "${SHADER_SRC_DIR}/${SHADER}"
            VERBATIM
    )
    list(APPEND COMPILED_SHADERS ${COMPILED_SHADER})
endforeach ()

# Delete any orphaned shaders
file(GLOB EXISTING_OUTPUTS "${DEST_DIR}/*.spv")
foreach(FILE ${EXISTING_OUTPUTS})
    if(NOT FILE IN_LIST COMPILED_SHADERS)
        file(REMOVE "${FILE}")
    endif()
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${COMPILED_SHADERS})

# Get ImGui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY	https://github.com/ocornut/imgui.git
        GIT_TAG			v1.92.9
)
FetchContent_MakeAvailable(imgui)

# Create Bin2H utility
add_executable(Bin2H "${imgui_SOURCE_DIR}/misc/fonts/binary_to_compressed_c.cpp")
set_target_properties(Bin2H PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

# Glob all resources
set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
file(GLOB_RECURSE RESOURCE_FILES CONFIGURE_DEPENDS RELATIVE "${RESOURCES_DIR}" "${RESOURCES_DIR}/*")

# Generate resources headers
set(RESOURCE_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_HEADERS "")
foreach (RESOURCE ${RESOURCE_FILES})
    get_filename_component(FILE_SUBPATH ${RESOURCE} DIRECTORY)
    get_filename_component(FILE_NAME ${RESOURCE} NAME_WE)
    string(REPLACE "/" "_" GENERATED_FILE_NAME "${FILE_SUBPATH}/${FILE_NAME}")
    set(GENERATED_FILE "${RESOURCE_DEST_DIR}/data/${GENERATED_FILE_NAME}.h")
    add_custom_command(
            OUTPUT "${GENERATED_FILE}"
            COMMAND Bin2H -nocompress "${RESOURCES_DIR}/${RESOURCE}" "${GENERATED_FILE_NAME}" > "${GENERATED_FILE}"
            DEPENDS CompileShaders "${RESOURCES_DIR}/${RESOURCE}"
            VERBATIM
    )

    list(APPEND GENERATED_HEADERS ${GENERATED_FILE})
endforeach ()

# Create a target for building resources
add_custom_target(BuildResources ALL DEPENDS ${GENERATED_HEADERS})

# Generate a header containing an enum of all resources and a get function definition
set(RESOURCE_DATA_HEADER "${RESOURCE_DEST_DIR}/resourceData.h")
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
set(RESOURCE_DATA_SOURCE "${RESOURCE_DEST_DIR}/resourceData.c")
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
target_include_directories(Resources PUBLIC "${RESOURCE_DEST_DIR}")
add_dependencies(Resources BuildResources)