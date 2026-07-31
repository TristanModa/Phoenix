include(FetchContent)

find_package(Git REQUIRED)
find_package(Vulkan REQUIRED)

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

# Get Bin2H
#FetchContent_Declare(
#		bin2h
#		GIT_REPOSITORY	https://github.com/sivachandran/cmake-bin2h.git
#		GIT_TAG			master
#)
#FetchContent_MakeAvailable(bin2h)
#include("${bin2h_SOURCE_DIR}/bin2h.cmake")
#
## Generate embedded resources for ImGui
#file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/embedded/")
#set(IMGUI_EMBEDDED_SHADER_HEADER "${CMAKE_CURRENT_BINARY_DIR}/embedded/imgui_sdl3gpu_shaders.h")
#if (NOT EXISTS "${IMGUI_EMBEDDED_SHADER_HEADER}")
#	# Compile shaders
#	set(IMGUI_VERT_SPV "${CMAKE_CURRENT_BINARY_DIR}/embedded/imgui_sdl3gpu_shader.vert.spv")
#	set(IMGUI_FRAG_SPV "${CMAKE_CURRENT_BINARY_DIR}/embedded/imgui_sdl3gpu_shader.frag.spv")
#	execute_process(
#			COMMAND ${Vulkan_GLSLC_EXECUTABLE}
#			"${imgui_SOURCE_DIR}/backends/sdlgpu3/shader.vert"
#			-o "${IMGUI_VERT_SPV}"
#
#			RESULT_VARIABLE VERT_RESULT
#			ERROR_VARIABLE VERT_ERROR
#	)
#	execute_process(
#			COMMAND ${Vulkan_GLSLC_EXECUTABLE}
#			"${imgui_SOURCE_DIR}/backends/sdlgpu3/shader.frag"
#			-o "${IMGUI_FRAG_SPV}"
#
#			RESULT_VARIABLE FRAG_RESULT
#			ERROR_VARIABLE FRAG_ERROR
#	)
#
#	# Check if compilation was successful
#	if (NOT VERT_RESULT EQUAL 0)
#		message(FATAL_ERROR "ImGui vertex shader compilation failed: ${VERT_ERROR}")
#	elseif (NOT FRAG_RESULT EQUAL 0)
#		message(FATAL_ERROR "ImGui fragment shader compilation failed: ${FRAG_ERROR}")
#	endif ()
#
#	# Generate header
#	bin2h(
#			SOURCE_FILE "${IMGUI_VERT_SPV}"
#			HEADER_FILE "${IMGUI_EMBEDDED_SHADER_HEADER}"
#			VARIABLE_NAME "IMGUI_VERTEX_SHADER"
#			APPEND
#	)
#	bin2h(
#			SOURCE_FILE "${IMGUI_FRAG_SPV}"
#			HEADER_FILE "${IMGUI_EMBEDDED_SHADER_HEADER}"
#			VARIABLE_NAME "IMGUI_FRAGMENT_SHADER"
#			APPEND
#	)
#endif ()

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