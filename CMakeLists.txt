cmake_minimum_required(VERSION 4.2)

include(FetchContent)

set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${CMAKE_BINARY_DIR}/_deps/vcpkg-src/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Create the project
project(Phoenix
        VERSION 0.0.0
        LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Get vcpkg
FetchContent_Declare(
        vcpkg
        GIT_REPOSITORY      https://github.com/microsoft/vcpkg.git
        GIT_TAG             2026.06.24
)
FetchContent_MakeAvailable(vcpkg)

# Set up vcpkg
set(VCPKG_EXECUTABLE "${vcpkg_SOURCE_DIR}/vcpkg${CMAKE_EXECUTABLE_SUFFIX}")
set(VCPKG_BOOTSTRAP_SCRIPT "${vcpkg_SOURCE_DIR}/bootstrap-vcpkg.bat")
if (NOT EXISTS "${VCPKG_EXECUTABLE}")
    execute_process(
            COMMAND             "${VCPKG_BOOTSTRAP_SCRIPT}"
            WORKING_DIRECTORY   "${vcpkg_SOURCE_DIR}"
            RESULT_VARIABLE     BOOTSTRAP_RESULT
    )
    if (NOT BOOTSTRAP_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to bootstrap vcpkg")
    endif ()
endif ()


add_subdirectory(src)
add_subdirectory(vendor)