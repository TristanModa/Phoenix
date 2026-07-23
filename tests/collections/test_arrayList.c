#include <criterion/criterion.h>
#include <collections/arrayList.h>

#include "core/memory.h"

// region Helpers
static int destroyCount;
static void intDestructor(void* _) {
    destroyCount++;
}
// endregion

// region ArrayList_create Tests
Test(Collections, ArrayList_create_Success) {
    const ArrayList* list = ArrayList_create(sizeof(int), 0, nullptr);
    cr_assert_not_null(list);
}

Test(Collections, ArrayList_create_ZeroItemSize) {
    const ArrayList* list = ArrayList_create(0, 0, nullptr);
    cr_assert_null(list);
}

Test(Collections, ArrayList_create_NonZeroCapacity) {
    const ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_eq(list->capacity, 16);
    cr_assert_not_null(list->items);
}

Test(Collections, ArrayList_create_FailedInstanceMalloc) {
    const ArrayList* list = ArrayList_create(sizeof(int), -1, nullptr);
    cr_assert_null(list);
}

Test(Collections, ArrayList_create_FailedItemsMalloc) {
    const ArrayList* list = ArrayList_create(-1, 16, nullptr);
    cr_assert_null(list);
}
// endregion

// region ArrayList_destroy
Test(Collections, ArrayList_destroy_NullList) {
    cr_assert_not(ArrayList_destroy(nullptr));
}

Test(Collections, ArrayList_destroy_NoItemDestructor) {
    // Get the memory before
    const size_t memBefore = Memory_getStats().currentAllocatedBytes;

    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Destroy the ArrayList
    const bool success = ArrayList_destroy(list);
    cr_assert(success);

    // Get the memory delta
    const size_t memAfter = Memory_getStats().currentAllocatedBytes;
    const size_t memDelta = memAfter - memBefore;
    cr_assert_eq(memDelta, 0);
}

Test(Collections, ArrayList_destroy_ItemDestructor) {
    // Reset destroy count
    destroyCount = 0;

    // Get the memory before
    const size_t memBefore = Memory_getStats().currentAllocatedBytes;

    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, intDestructor);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Destroy the ArrayList
    const bool success = ArrayList_destroy(list);
    cr_assert(success);

    // Get the memory delta
    const size_t memAfter = Memory_getStats().currentAllocatedBytes;
    const size_t memDelta = memAfter - memBefore;
    cr_assert_eq(memDelta, 0);

    // Check destroy count
    cr_assert_eq(destroyCount, 10);
}
// endregion

// region ArrayList_getLength Tests
Test(Collections, ArrayList_getLength_NullList) {
    cr_assert_eq(ArrayList_getLength(nullptr), 0);
}

Test(Collections, ArrayList_getLength_Empty) {
    const ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_eq(ArrayList_getLength(list), 0);
}

Test(Collections, ArrayList_getLength_HasItems) {
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }
    cr_assert_eq(ArrayList_getLength(list), 10);
}
// endregion

// region ArrayList_getCapacity Tests
Test(Collections, ArrayList_getCapacity_NullList) {
    cr_assert_eq(ArrayList_getCapacity(nullptr), 0);
}

Test(Collections, ArrayList_getCapacity_ZeroCap) {
    const ArrayList* list = ArrayList_create(sizeof(int), 0, nullptr);
    cr_assert_eq(ArrayList_getCapacity(list), 0);
}

Test(Collections, ArrayList_getCapacity_NonZeroCap) {
    const ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_eq(ArrayList_getCapacity(list), 16);
}
// endregion

// region ArrayList_clear Tests
Test(Collections, ArrayList_clear_NullList) {
    cr_assert_not(ArrayList_clear(nullptr));
}

Test(Collections, ArrayList_clear_Empty) {
    ArrayList* list = ArrayList_create(sizeof(int), 0, nullptr);
    const bool success = ArrayList_clear(list);
    cr_assert(success);
}

Test(Collections, ArrayList_clear_HasItems) {
    // Reset destroy count
    destroyCount = 0;

    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, intDestructor);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Clear the ArrayList
    const bool success = ArrayList_clear(list);
    cr_assert(success);

    // Check list length
    cr_assert_eq(list->length, 0);

    // Check destructor call count
    cr_assert_eq(destroyCount, 10);
}
// endregion

// region ArrayList_resize Tests
Test(Collections, ArrayList_resize_NullList) {
    cr_assert_not(ArrayList_resize(nullptr, 0));
}

Test(Collections, ArrayList_resize_CapacityLessThanLength) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Get state before
    const ArrayList stateBefore = *list;

    // Resize the ArrayList
    const bool success = ArrayList_resize(list, 8);

    // Check failure with no state change
    cr_assert_not(success);
    cr_assert_arr_eq(list, &stateBefore, sizeof(ArrayList));
}

Test(Collections, ArrayList_resize_FailedInstanceMalloc) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Get state before
    const ArrayList stateBefore = *list;

    // Resize the ArrayList
    const bool success = ArrayList_resize(list, -1);

    // Check failure with no state change
    cr_assert_not(success);
    cr_assert_arr_eq(list, &stateBefore, sizeof(ArrayList));
}

Test(Collections, ArrayList_resize_ZeroCapacity) {
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    const bool success = ArrayList_resize(list, 0);
    cr_assert(success);
    cr_assert_eq(list->capacity, 0);
    cr_assert_eq(list->items, nullptr);
}

Test(Collections, ArrayList_resize_NonZeroCapacity) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    int items[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &items[i]);
    }

    // Resize the ArrayList
    const bool success = ArrayList_resize(list, 32);

    // Check success
    cr_assert(success);
    cr_assert_eq(list->capacity, 32);
    cr_assert_not_null(list->items);
    cr_assert_arr_eq(list->items, items, list->length * sizeof(int));
}
// endregion

// region ArrayList_shrink Tests
Test(Collections, ArrayList_shrink_NullList) {
    cr_assert_not(ArrayList_resize(nullptr, 0));
}

Test(Collections, ArrayList_shrink_Success) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Resize the ArrayList
    const bool success = ArrayList_shrink(list);

    // Check success
    cr_assert(success);
    cr_assert_eq(list->capacity, 10);
}
// endregion

// region ArrayList_getItem Tests
Test(Collections, ArrayList_getItem_NullList) {
    cr_assert_null(ArrayList_getItem(nullptr, 0));
}

Test(Collections, ArrayList_getItem_IndexOOB) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Get the item
    cr_assert_null(ArrayList_getItem(nullptr, 11));
}

Test(Collections, ArrayList_getItem_Success) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Get each item
    for (int i = 0; i < 10; i++) {
        const int* item = ArrayList_getItem(list, i);
        cr_assert_eq(*item, i);
    }
}
// endregion

// region ArrayList_insertItem Tests
Test(Collections, ArrayList_insertItem_NullList) {
    int i = 0;
    cr_assert_null(ArrayList_insertItem(nullptr, 0, &i));
}

Test(Collections, ArrayList_insertItem_NullItem) {
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_null(ArrayList_insertItem(list, 0, nullptr));
}

Test(Collections, ArrayList_insertItem_First) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Insert the item
    int i = -1;
    const int* item = ArrayList_insertItem(list, 0, &i);

    // Check success
    constexpr int expected[] = { -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    cr_assert_not_null(item);
    cr_assert_eq(*item, i);
    cr_assert_arr_eq(list->items, expected, 11 * sizeof(int));
}

Test(Collections, ArrayList_insertItem_Middle) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Insert the item
    int i = -1;
    const int* item = ArrayList_insertItem(list, 5, &i);

    // Check success
    constexpr int expected[] = { 0, 1, 2, 3, 4, -1, 5, 6, 7, 8, 9 };
    cr_assert_not_null(item);
    cr_assert_eq(*item, i);
    cr_assert_arr_eq(list->items, expected, 11 * sizeof(int));
}

Test(Collections, ArrayList_insertItem_Last) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Insert the item
    int i = -1;
    const int* item = ArrayList_insertItem(list, 10, &i);

    // Check success
    constexpr int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1 };
    cr_assert_not_null(item);
    cr_assert_eq(*item, i);
    cr_assert_arr_eq(list->items, expected, 11 * sizeof(int));
}

Test(Collections, ArrayList_insertItem_RequiresResize) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 4, nullptr);
    for (int i = 0; i < 4; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Insert item
    int i = 0;
    const int* item = ArrayList_insertItem(list, 0, &i);

    // Check success
    cr_assert_not_null(item);
    cr_assert_eq(*item, i);
    cr_assert_eq(list->capacity, 8);
    cr_assert_not_null(list->items);
}
// endregion

// region ArrayList_removeItem Tests

// endregion