cmake_policy(SET CMP0169 OLD)

include(FetchContent)
include(ExternalProject)

# Get SDL3
FetchContent_Declare(
        sdl3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.0
)
FetchContent_MakeAvailable(sdl3)

# Get Mimalloc
FetchContent_Declare(
		mimalloc
		GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
		GIT_TAG v3.4.3
)
FetchContent_MakeAvailable(mimalloc)

# Get Collections-C
FetchContent_Declare(
		collectionsC
		GIT_REPOSITORY https://github.com/srdja/Collections-C.git
		GIT_TAG dbedaf9
		SOURCE_SUBDIR "disable"
)
FetchContent_MakeAvailable(collectionsC)

# Build Collections-C
file(GLOB_RECURSE COLLECTIONS_C_SOURCES "${collectionsC_SOURCE_DIR}/**/*.c")
add_library(CollectionsC STATIC ${COLLECTIONS_C_SOURCES})
target_include_directories(CollectionsC PUBLIC "${collectionsC_SOURCE_DIR}/include")

# Create the vendor interface
add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE
        SDL3::SDL3
		mimalloc-static
		CollectionsC
)