include(FetchContent)

# Get SDL3 suite
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)

# Create the vendor interface
add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE
        SDL3::SDL3
        SDL3_image::SDL3_image
)