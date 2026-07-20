include(FetchContent)

# Get BoxFort
FetchContent_Declare(
        boxfort
        GIT_REPOSITORY https://github.com/Snaipe/BoxFort
        GIT_TAG        v0.1.5
)
FetchContent_MakeAvailable(boxfort)

# Build BoxFort
file(GLOB_RECURSE BOX_FORT_SRC_FILES CONFIGURE_DEPENDS "${boxfort_SOURCE_DIR}/src/*.c")
add_library(BoxFort ${BOX_FORT_SRC_FILES})
target_include_directories(BoxFort PUBLIC "${boxfort_SOURCE_DIR}/include")

# Get Criterion
FetchContent_Declare(
        criterion
        GIT_REPOSITORY https://github.com/snaipe/criterion
        GIT_TAG        v2.4.3
)
FetchContent_MakeAvailable(criterion)

# Build Criterion
file(GLOB_RECURSE CRITERION_SRC_FILES CONFIGURE_DEPENDS "${criterion_SOURCE_DIR}/src/*.c")
add_library(Criterion ${CRITERION_SRC_FILES})
target_link_libraries(Criterion PRIVATE BoxFort)
target_include_directories(Criterion PUBLIC "${criterion_SOURCE_DIR}/include")

# Add subdirectories
add_subdirectory(collections)