include(FetchContent)
include(ExternalProject)

# Get SDL3
FetchContent_Declare(
        sdl3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.0
)
FetchContent_MakeAvailable(sdl3)

# Create the vendor interface
add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE
        SDL3::SDL3
)