set(COLLECTIONS_TESTS "VulfEngine_Collections_Tests")
add_executable(${COLLECTIONS_TESTS}
        test_arrayList.c
)
target_link_libraries(${COLLECTIONS_TESTS} VulfEngine Criterion)
target_include_directories(${COLLECTIONS_TESTS} PRIVATE "${CMAKE_SOURCE_DIR}/engine")