include(FetchContent)

find_package(Git REQUIRED)

# Get SDL3
FetchContent_Declare(
		sdl3
		GIT_REPOSITORY 	"https://github.com/libsdl-org/SDL.git"
		GIT_TAG 		"release-3.4.0"
)
FetchContent_MakeAvailable(sdl3)

# Get Mimalloc
FetchContent_Declare(
		mimalloc
		GIT_REPOSITORY 	"https://github.com/microsoft/mimalloc.git"
		GIT_TAG 		"v3.4.3"
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
file(GLOB_RECURSE COLLECTIONS_C_SOURCES "${COLLECTIONS_C_SOURCE_DIR}/src/*.c")
add_library(CollectionsC STATIC ${COLLECTIONS_C_SOURCES})
target_include_directories(CollectionsC PUBLIC "${COLLECTIONS_C_SOURCE_DIR}/src/include")

# Get DearBindings
FetchContent_Declare(
		dearBindings
		URL				"https://github.com/dearimgui/dear_bindings/releases/download/DearBindings_v0.21_ImGui_v1.92.9/DearBindings_v0.21_ImGui_v1.92.9.zip"
		URL_HASH		SHA256=4e89f160b6480122d1ed798182bb96c957633fa1bc73046cf5cea961743f874a
)
FetchContent_MakeAvailable(dearBindings)

# Get ImGui
FetchContent_Declare(
		imgui
		GIT_REPOSITORY	https://github.com/ocornut/imgui.git
		GIT_TAG			v1.92.9
)
FetchContent_MakeAvailable(imgui)

# Build ImGui-C
file(GLOB DEAR_BINDINGS_SOURCES "${dearbindings_SOURCE_DIR}/*.cpp")
file(GLOB IMGUI_SOURCES "${imgui_SOURCE_DIR}/*.cpp")
add_library(ImGui-C
		"${DEAR_BINDINGS_SOURCES}"
		"${IMGUI_SOURCES}"
		"${dearbindings_SOURCE_DIR}/backends/dcimgui_impl_sdl3.cpp"
		"${dearbindings_SOURCE_DIR}/backends/dcimgui_impl_sdlgpu3.cpp"
		"${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
		"${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp"
)
target_link_libraries(ImGui-C PRIVATE SDL3::SDL3)
target_include_directories(ImGui-C PUBLIC
		"${dearbindings_SOURCE_DIR}"
		"${dearbindings_SOURCE_DIR}/backends"
		"${CMAKE_CURRENT_BINARY_DIR}/embedded/"
		"${imgui_SOURCE_DIR}"
		"${imgui_SOURCE_DIR}/backends"
)

# Create the vendor interface
add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE
		SDL3::SDL3
		mimalloc-static
		CollectionsC
		ImGui-C
)