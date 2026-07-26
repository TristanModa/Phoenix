cmake_policy(SET CMP0169 OLD)

include(FetchContent)

find_package(Git REQUIRED)

# Get SDL3
FetchContent_Declare(
        sdl3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.0
)
FetchContent_MakeAvailable(sdl3)

# Get SDL_Image
FetchContent_Declare(
		sdl3-image
		GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
		GIT_TAG release-3.4.4
)
FetchContent_MakeAvailable(sdl3-image)

# Get Mimalloc
FetchContent_Declare(
		mimalloc
		GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
		GIT_TAG v3.4.3
)
FetchContent_MakeAvailable(mimalloc)

# Get Collections-C
set(COLLECTIONS_C_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/collectionsc")
if (NOT EXISTS "${COLLECTIONS_C_SOURCE_DIR}/.git")
	execute_process(
			COMMAND ${GIT_EXECUTABLE} clone
			https://github.com/srdja/Collections-C.git
			${COLLECTIONS_C_SOURCE_DIR}
			RESULT_VARIABLE result
	)

	execute_process(
			COMMAND ${GIT_EXECUTABLE} checkout dbedaf9
			WORKING_DIRECTORY ${COLLECTIONS_C_SOURCE_DIR}
	)
endif ()

# Build Collections-C
message(${COLLECTIONS_C_SOURCE_DIR})
file(GLOB_RECURSE COLLECTIONS_C_SOURCES "${COLLECTIONS_C_SOURCE_DIR}/src/*.c")
add_library(CollectionsC STATIC ${COLLECTIONS_C_SOURCES})
target_include_directories(CollectionsC PUBLIC "${COLLECTIONS_C_SOURCE_DIR}/src/include")

# Create the vendor interface
add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE
        SDL3::SDL3
		SDL3_image::SDL3_image
		mimalloc-static
		CollectionsC
)