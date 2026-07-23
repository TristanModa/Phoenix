include(FetchContent)

enable_testing()

if(WIN32)
    message(STATUS "Skipping tests: Criterion is not supported on Windows.")
    return()
endif()

# Get Criterion
FetchContent_Declare(
        criterion
        URL         https://github.com/Snaipe/Criterion/releases/download/v2.4.3/criterion-2.4.3-linux-x86_64.tar.xz
        URL_HASH    SHA256=f1b3dd5186783dcdd63433c1facd3b4d6af5244a151057370b53bdda80f16121
)
FetchContent_MakeAvailable(criterion)

# Add Criterion
add_library(Criterion SHARED IMPORTED)
set_target_properties(Criterion PROPERTIES
        IMPORTED_LOCATION "${criterion_SOURCE_DIR}/lib/libcriterion.so"
)
target_include_directories(Criterion INTERFACE "${criterion_SOURCE_DIR}/include")

# Create the test executable
file(GLOB_RECURSE TEST_SOURCES *.c)
add_executable(VulfEngine_Tests ${TEST_SOURCES})
target_link_libraries(VulfEngine_Tests PRIVATE VulfEngine Criterion)
target_include_directories(VulfEngine_Tests PRIVATE "${CMAKE_SOURCE_DIR}/engine")