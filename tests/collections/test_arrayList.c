#include <criterion/criterion.h>

#include <core/memory.h>
#include <collections/arrayList.h>

// region Helpers
typedef struct testStruct {
    int shallowData;
    int* deepData;
} TestStruct;

static void exampleDestructor(TestStruct* testStruct) {
    Memory_free(testStruct->deepData);
}
// endregion

// region Create Tests
Test(Collections, ArrayList_Create_Success) {
    const ArrayList* arrayList = ArrayList_create(sizeof(int), 0, nullptr);
    cr_assert_not_null(arrayList);
}

Test(Collections, ArrayList_Create_ZeroItemSize) {
    const ArrayList* arrayList = ArrayList_create(0, 0, nullptr);
    cr_assert_null(arrayList);
}

Test(Collections, ArrayList_Create_NonZeroCapacity) {
    const ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_eq(arrayList->capacity, 16);
    cr_assert_not_null(arrayList->items);
}

Test(Collections, ArrayList_Create_FailedInstanceMalloc) {
    const ArrayList* arrayList = ArrayList_create(sizeof(int), -1, nullptr);
    cr_assert_null(arrayList);
}

Test(Collections, ArrayList_Create_FailedItemsMalloc) {
    const ArrayList* arrayList = ArrayList_create(-1, 16, nullptr);
    cr_assert_null(arrayList);
}
// endregion

// region Destroy Tests
Test(Collections, ArrayList_Destroy_Empty) {
    // Get the memory before allocation
    const size_t memBefore = Memory_getStats().currentAllocatedBytes;

    // Create and destroy an ArrayList
    ArrayList* arrayList = ArrayList_create(sizeof(int), 0, nullptr);
    ArrayList_destroy(arrayList);

    // Get the memory after destruction
    const size_t memAfter = Memory_getStats().currentAllocatedBytes;

    // Check if memBefore is equal to memAfter
    cr_assert_eq(memBefore, memAfter);
}

Test(Collections, ArrayList_Destroy_HasItems) {
    // Get the memory before allocation
    const size_t memBefore = Memory_getStats().currentAllocatedBytes;

    // Create and destroy an ArrayList
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    ArrayList_destroy(arrayList);

    // Get the memory after destruction
    const size_t memAfter = Memory_getStats().currentAllocatedBytes;

    // Check if memBefore is equal to memAfter
    cr_assert_eq(memBefore, memAfter);
}

Test(Collections, ArrayList_Destroy_HasDeepItemsNoDestructor) {
    // Get the memory before allocation
    const size_t memBefore = Memory_getStats().currentAllocatedBytes;

    // Create, insert to, and destroy an ArrayList
    ArrayList* arrayList = ArrayList_create(sizeof(TestStruct), 16, nullptr);
    const TestStruct a = {
        .shallowData = 0,
        .deepData = Memory_malloc(sizeof(a.deepData))
    };
    ArrayList_insertItem(arrayList, &a, 0);
    ArrayList_destroy(arrayList);

    // Get the memory after destruction
    const size_t memAfter = Memory_getStats().currentAllocatedBytes;

    // Check if memBefore is equal to memAfter
    cr_assert_eq(memBefore, memAfter - sizeof(a.deepData));
}

Test(Collections, ArrayList_Destroy_HasDeepItemsDestructor) {
    // Get the memory before allocation
    const size_t memBefore = Memory_getStats().currentAllocatedBytes;

    // Create, insert to, and destroy an ArrayList
    ArrayList* arrayList = ArrayList_create(sizeof(TestStruct), 16, (CollectionsItemDestructorFn)exampleDestructor);
    const TestStruct a = {
        .shallowData = 0,
        .deepData = Memory_malloc(sizeof(a.deepData))
    };
    ArrayList_insertItem(arrayList, &a, 0);
    ArrayList_destroy(arrayList);

    // Get the memory after destruction
    const size_t memAfter = Memory_getStats().currentAllocatedBytes;

    // Check if memBefore is equal to memAfter
    cr_assert_eq(memBefore, memAfter);
}
// endregion Destroy Tests