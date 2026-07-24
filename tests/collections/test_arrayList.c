#include <criterion/criterion.h>
#include <collections/arrayList.h>

// region Helpers
static int sum;
static void actionFunc(void* item) {
    sum += *(int*)item;
}
static int compareFunc(const int* a, const int* b) {
    return (*a > *b) - (*a < *b);
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
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Destroy the ArrayList
    const bool success = ArrayList_destroy(list);
    cr_assert(success);
}

Test(Collections, ArrayList_destroy_ItemDestructor) {
    sum = 0;

    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, actionFunc);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Destroy the ArrayList
    const bool success = ArrayList_destroy(list);
    cr_assert(success);
    cr_assert_eq(sum, 45);
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
    sum = 0;

    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, actionFunc);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Clear the ArrayList
    const bool success = ArrayList_clear(list);
    cr_assert(success);

    // Check list length
    cr_assert_eq(list->length, 0);

    // Check destructor called on each
    cr_assert_eq(sum, 45);
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
    cr_assert_not(ArrayList_shrink(nullptr));
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
    cr_assert_null(ArrayList_getItem(list, 10));
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

Test(Collections, ArrayList_insertItem_IndexOOB) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Get state before
    ArrayList stateBefore = *list;

    // Insert the item
    int i = -1;
    const int* item = ArrayList_insertItem(list, 11, &i);

    // Check failure with no state change
    cr_assert_null(item);
    cr_assert_arr_eq(list, &stateBefore, sizeof(ArrayList));
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
    cr_assert_eq(list->length, 11);
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
    cr_assert_eq(list->length, 11);
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
    cr_assert_eq(list->length, 11);
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
    cr_assert_eq(list->length, 5);
    cr_assert_eq(*item, i);
    cr_assert_eq(list->capacity, 8);
    cr_assert_not_null(list->items);
}
// endregion

// region ArrayList_removeItem Tests
Test(Collections, ArrayList_removeItem_NullList) {
    int out = -1;
    cr_assert_not(ArrayList_removeItem(nullptr, 0, &out));
    cr_assert_eq(out, -1);
}

Test(Collections, ArrayList_removeItem_IndexOOB) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Get state before
    ArrayList stateBefore = *list;

    // Remove the item
    int out = -1;
    bool success = ArrayList_removeItem(list, 10, &out);

    // Check failure with no state change
    cr_assert_not(success);
    cr_assert_eq(out, -1);
    cr_assert_arr_eq(list, &stateBefore, sizeof(ArrayList));
}

Test(Collections, ArrayList_removeItem_First) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Remove the item
    int out = -1;
    bool success = ArrayList_removeItem(list, 0, &out);

    // Check success
    constexpr int expected[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    cr_assert(success);
    cr_assert_eq(out, 0);
    cr_assert_eq(list->length, 9);
    cr_assert_arr_eq(list->items, expected, 9 * sizeof(int));
}

Test(Collections, ArrayList_removeItem_Middle) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Remove the item
    int out = -1;
    bool success = ArrayList_removeItem(list, 5, &out);

    // Check success
    constexpr int expected[] = { 0, 1, 2, 3, 4, 6, 7, 8, 9 };
    cr_assert(success);
    cr_assert_eq(out, 5);
    cr_assert_eq(list->length, 9);
    cr_assert_arr_eq(list->items, expected, 9 * sizeof(int));
}

Test(Collections, ArrayList_removeItem_Last) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Remove the item
    int out = -1;
    const bool success = ArrayList_removeItem(list, 9, &out);

    // Check success
    constexpr int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    cr_assert(success);
    cr_assert_eq(out, 9);
    cr_assert_eq(list->length, 9);
    cr_assert_arr_eq(list->items, expected, 9 * sizeof(int));
}

Test(Collections, ArrayList_removeItem_NullOut) {
    sum = 0;

    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, actionFunc);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Remove the item
    const bool success = ArrayList_removeItem(list, 5, nullptr);

    // Check success
    constexpr int expected[] = { 0, 1, 2, 3, 4, 6, 7, 8, 9 };
    cr_assert(success);
    cr_assert_eq(sum, 5);
    cr_assert_eq(list->length, 9);
    cr_assert_arr_eq(list->items, expected, 9 * sizeof(int));
}
// endregion

// region ArrayList_replaceItem Tests
Test(Collections, ArrayList_replaceItem_NullList) {
    int i = 0;
    int out = -1;
    cr_assert_null(ArrayList_replaceItem(nullptr, 0, &i, &i));
    cr_assert_eq(out, -1);
}

Test(Collections, ArrayList_replaceItem_NullItem) {
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_null(ArrayList_replaceItem(list, 0, nullptr, nullptr));
}

Test(Collections, ArrayList_replaceItem_IndexOOB) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Get state before
    ArrayList stateBefore = *list;

    // Remove the item
    int i = 0;
    int out = -1;
    const void* success = ArrayList_replaceItem(list, 10, &i, &out);

    // Check failure with no state change
    cr_assert_not(success);
    cr_assert_eq(out, -1);
    cr_assert_arr_eq(list, &stateBefore, sizeof(ArrayList));
}

Test(Collections, ArrayList_replaceItem_First) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Replace the item
    int i = -1;
    int out = -2;
    const int* item = ArrayList_replaceItem(list, 0, &i, &out);

    // Check success
    constexpr int expected[] = { -1, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    cr_assert_not_null(item);
    cr_assert_eq(*item, i);
    cr_assert_eq(out, 0);
    cr_assert_eq(list->length, 10);
    cr_assert_arr_eq(list->items, expected, 10 * sizeof(int));
}

Test(Collections, ArrayList_replaceItem_Middle) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Replace the item
    int i = -1;
    int out = -2;
    const int* item = ArrayList_replaceItem(list, 5, &i, &out);

    // Check success
    constexpr int expected[] = { 0, 1, 2, 3, 4, -1, 6, 7, 8, 9 };
    cr_assert_not_null(item);
    cr_assert_eq(*item, i);
    cr_assert_eq(out, 5);
    cr_assert_eq(list->length, 10);
    cr_assert_arr_eq(list->items, expected, 10 * sizeof(int));
}

Test(Collections, ArrayList_replaceItem_Last) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Replace the item
    int i = -1;
    int out = -2;
    const int* item = ArrayList_replaceItem(list, 9, &i, &out);

    // Check success
    constexpr int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, -1 };
    cr_assert_not_null(item);
    cr_assert_eq(*item, i);
    cr_assert_eq(out, 9);
    cr_assert_eq(list->length, 10);
    cr_assert_arr_eq(list->items, expected, 10 * sizeof(int));
}
// endregion

// region ArrayList_pushBackItem Tests
Test(Collections, ArrayList_pushBackItem_NullList) {
    int i = 0;
    cr_assert_null(ArrayList_pushBackItem(nullptr, &i));
}

Test(Collections, ArrayList_pushBackItem_NullItem) {
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_null(ArrayList_pushBackItem(list, nullptr));
}

Test(Collections, ArrayList_pushBackItem_Success) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Insert the item
    int i = -1;
    const int* item = ArrayList_pushBackItem(list, &i);

    // Check success
    constexpr int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1 };
    cr_assert_not_null(item);
    cr_assert_eq(list->length, 11);
    cr_assert_eq(*item, i);
    cr_assert_arr_eq(list->items, expected, 11 * sizeof(int));
}

Test(Collections, ArrayList_pushBackItem_RequiresResize) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 4, nullptr);
    for (int i = 0; i < 4; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Insert item
    int i = 0;
    const int* item = ArrayList_pushBackItem(list, &i);

    // Check success
    cr_assert_not_null(item);
    cr_assert_eq(list->length, 5);
    cr_assert_eq(*item, i);
    cr_assert_eq(list->capacity, 8);
    cr_assert_not_null(list->items);
}
// endregion

// region ArrayList_popBackItem Tests
Test(Collections, ArrayList_popBackItem_NullList) {
    int out = -1;
    cr_assert_not(ArrayList_popBackItem(nullptr, &out));
    cr_assert_eq(out, -1);
}

Test(Collections, ArrayList_popBackItem_Success) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Remove the item
    int out = -1;
    bool success = ArrayList_popBackItem(list, &out);

    // Check success
    constexpr int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    cr_assert(success);
    cr_assert_eq(out, 9);
    cr_assert_eq(list->length, 9);
    cr_assert_arr_eq(list->items, expected, 9 * sizeof(int));
}

Test(Collections, ArrayList_popBackItem_NullOut) {
    sum = 0;

    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, actionFunc);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Remove the item
    const bool success = ArrayList_popBackItem(list, nullptr);

    // Check success
    constexpr int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    cr_assert(success);
    cr_assert_eq(sum, 9);
    cr_assert_eq(list->length, 9);
    cr_assert_arr_eq(list->items, expected, 9 * sizeof(int));
}
// endregion

// region ArrayList_forEach Tests
Test(Collections, ArrayList_forEach_NullList) {
    cr_assert_not(ArrayList_forEach(nullptr, actionFunc));
}

Test(Collections, ArrayList_forEach_NullAction) {
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_not(ArrayList_forEach(list, nullptr));
}

Test(Collections, ArrayList_forEach_Success) {
    sum = 0;

    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Iterate the list
    const bool success = ArrayList_forEach(list, actionFunc);

    // Check action called on each
    cr_assert(success);
    cr_assert_eq(sum, 45);
}
// endregion

// region ArrayList_find
Test(Collections, ArrayList_find_NullList) {
    constexpr int key = 0;
    cr_assert_null(ArrayList_find(nullptr, &key, (CollectionsCompareFn)compareFunc));
}

Test(Collections, ArrayList_find_NullKey) {
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_null(ArrayList_find(list, nullptr, (CollectionsCompareFn)compareFunc));
}

Test(Collections, ArrayList_find_NullCompareFunc) {
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    int key = 0;
    cr_assert_null(ArrayList_find(list, &key, nullptr));
}

Test(Collections, ArrayList_find_Success) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Find the key
    constexpr int key = 5;
    const int* item = ArrayList_find(list, &key, (CollectionsCompareFn)compareFunc);
    cr_assert_not_null(item);
    cr_assert_eq(*item, key);
}

Test(Collections, ArrayList_find_NotFound) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Find the key
    int key = -1;
    int* item = ArrayList_find(list, &key, (CollectionsCompareFn)compareFunc);
    cr_assert_null(item);
}
// endregion

// region ArrayList_sort Tests
Test(Collections, ArrayList_sort_NullList) {
    cr_assert_not(ArrayList_sort(nullptr, (CollectionsCompareFn)compareFunc));
}

Test(Collections, ArrayList_sort_NullCompareFunc) {
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_not(ArrayList_sort(list, nullptr));
}

Test(Collections, ArrayList_sort_Success) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    int unsorted[10] = { 5, 6, 9, 2, 1, 7, 4, 8, 3, 0 };
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &unsorted[i]);
    }

    // Sort the list
    bool success = ArrayList_sort(list, (CollectionsCompareFn)compareFunc);
    cr_assert(success);

    // Check sorted
    int expected[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    cr_assert_arr_eq(list->items, expected, 10);

}
// endregion

// region ArrayListIterator_begin Tests
Test(Collections, ArrayListIterator_begin_NullList) {
    ArrayListIterator it = ArrayList_begin(nullptr);
    ArrayListIterator expected = {};
    cr_assert_arr_eq(&it, &expected, sizeof(it));
}

Test(Collections, ArrayListIterator_begin_Success) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Create the iterator
    ArrayListIterator it = ArrayList_begin(list);
    ArrayListIterator expected = {
        .arrayList = list, .currentIndex = 0, .lastReturnedIndex = SIZE_MAX
    };
    cr_assert_arr_eq(&it, &expected, sizeof(it));
}
// endregion

// region ArrayListIterator_end Tests
Test(Collections, ArrayListIterator_end_NullList) {
    ArrayListIterator it = ArrayList_end(nullptr);
    ArrayListIterator expected = {};
    cr_assert_arr_eq(&it, &expected, sizeof(it));
}

Test(Collections, ArrayListIterator_end_Success) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Create the iterator
    ArrayListIterator it = ArrayList_end(list);
    ArrayListIterator expected = {
        .arrayList = list, .currentIndex = list->length, .lastReturnedIndex = SIZE_MAX
    };
    cr_assert_arr_eq(&it, &expected, sizeof(it));
}
// endregion

// region ArrayListIterator_next Tests
Test(Collections, ArrayListIterator_next_NullIterator) {
    cr_assert_null(ArrayListIterator_next(nullptr));
}

Test(Collections, ArrayListIterator_next_BadIteratorNullList) {
    ArrayListIterator it = {};
    cr_assert_null(ArrayListIterator_next(&it));
}

Test(Collections, ArrayListIterator_next_BadIteratorIndexOOB) {
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    ArrayListIterator it = { .arrayList = list, 1, .lastReturnedIndex = SIZE_MAX };
    cr_assert_null(ArrayListIterator_next(&it));
}

Test(Collections, ArrayListIterator_next_Advance) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Advance iterator
    ArrayListIterator it = ArrayList_begin(list);
    it.currentIndex = 5;
    int* item = ArrayListIterator_next(&it);

    // Check success
    cr_assert_not_null(item);
    cr_assert_eq(*item, 5);
    cr_assert_eq(it.currentIndex, 6);
    cr_assert_eq(it.lastReturnedIndex, 5);
}

Test(Collections, ArrayListIterator_next_EndOfList) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Advance iterator
    ArrayListIterator it = ArrayList_end(list);
    int* item = ArrayListIterator_next(&it);

    // Check success
    cr_assert_null(item);
    cr_assert_eq(it.currentIndex, 10);
    cr_assert_eq(it.lastReturnedIndex, SIZE_MAX);
}
// endregion

// region ArrayListIterator_previous Tests
Test(Collections, ArrayListIterator_previous_NullIterator) {
    cr_assert_null(ArrayListIterator_previous(nullptr));
}

Test(Collections, ArrayListIterator_previous_BadIteratorNullList) {
    ArrayListIterator it = {};
    cr_assert_null(ArrayListIterator_previous(&it));
}

Test(Collections, ArrayListIterator_previous_BadIteratorIndexOOB) {
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    ArrayListIterator it = { .arrayList = list, 1, .lastReturnedIndex = SIZE_MAX };
    cr_assert_null(ArrayListIterator_previous(&it));
}

Test(Collections, ArrayListIterator_previous_Regress) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Advance iterator
    ArrayListIterator it = ArrayList_end(list);
    it.currentIndex = 5;
    int* item = ArrayListIterator_previous(&it);

    // Check success
    cr_assert_not_null(item);
    cr_assert_eq(*item, 4);
    cr_assert_eq(it.currentIndex, 4);
    cr_assert_eq(it.lastReturnedIndex, 5);
}

Test(Collections, ArrayListIterator_previous_EndOfList) {
    // Populate the ArrayList
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    // Advance iterator
    ArrayListIterator it = ArrayList_begin(list);
    int* item = ArrayListIterator_previous(&it);

    // Check success
    cr_assert_null(item);
    cr_assert_eq(it.currentIndex, 0);
    cr_assert_eq(it.lastReturnedIndex, SIZE_MAX);
}
// endregion

// region ArrayListIterator_insertItem Tests

// endregion