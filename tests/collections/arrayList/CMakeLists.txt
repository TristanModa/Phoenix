set(TEST_TARGET "VulfEngine_Collections_ArrayList_Tests")

add_executable(${TEST_TARGET} test_arrayList.c)
target_link_libraries(${TEST_TARGET} PRIVATE VulfEngine Criterion)

add_test(NAME VulfEngine_Collections_ArrayList_Tests COMMAND VulfEngine_Test_Collections_ArrayList)