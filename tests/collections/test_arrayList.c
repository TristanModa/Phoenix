#include <criterion/criterion.h>
#include <collections/arrayList.h>

// region ArrayList_create Tests
Test(Collections, ArrayList_create_Success) {
    const ArrayList* arrayList = ArrayList_create(sizeof(int), 0, nullptr);
    cr_assert_not_null(arrayList);
}

Test(Collections, ArrayList_create_ZeroItemSize) {
    const ArrayList* arrayList = ArrayList_create(0, 0, nullptr);
    cr_assert_null(arrayList);
}

Test(Collections, ArrayList_create_NonZeroCapacity) {
    const ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_eq(arrayList->capacity, 16);
    cr_assert_not_null(arrayList->items);
}

Test(Collections, ArrayList_create_FailedInstanceMalloc) {
    const ArrayList* arrayList = ArrayList_create(sizeof(int), -1, nullptr);
    cr_assert_null(arrayList);
}

Test(Collections, ArrayList_create_FailedItemsMalloc) {
    const ArrayList* arrayList = ArrayList_create(-1, 16, nullptr);
    cr_assert_null(arrayList);
}
// endregion

// region ArrayList_destroy
Test(Collections, ArrayList_destroy_NullList) {
    ArrayList_destroy(nullptr);
    cr_assert(true);
}

Test(Collections, ArrayList_destroy_Success) {
    ArrayList* list = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }
    ArrayList_destroy(list);
    cr_assert(true);
}

static int destroyCount;
static void destroyInt(void* item) {
    (void)item;
    destroyCount++;
}

Test(Collections, ArrayList_destroy_CallsDestructor) {
    destroyCount = 0;

    ArrayList* list = ArrayList_create(sizeof(int), 8, destroyInt);
    for (int i = 0; i < 5; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    ArrayList_destroy(list);

    cr_assert_eq(destroyCount, 5);
}
// endregion

// region ArrayList_getLength Tests
Test(Collections, ArrayList_getLength_NullList) {
    cr_assert_eq(ArrayList_getLength(nullptr), 0);
}

Test(Collections, ArrayList_getLength_Empty) {
    const ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_eq(ArrayList_getLength(arrayList), 0);
}

Test(Collections, ArrayList_getLength_HasItems) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(arrayList, &i);
    }
    cr_assert_eq(ArrayList_getLength(arrayList), 10);
}
// endregion

// region ArrayList_getCapacity Tests
Test(Collections, ArrayList_getCapacity_NullList) {
    cr_assert_eq(ArrayList_getCapacity(nullptr), 0);
}

Test(Collections, ArrayList_getCapacity_ZeroCap) {
    const ArrayList* arrayList = ArrayList_create(sizeof(int), 0, nullptr);
    cr_assert_eq(ArrayList_getCapacity(arrayList), 0);
}

Test(Collections, ArrayList_getCapacity_NonZeroCap) {
    const ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    cr_assert_eq(ArrayList_getCapacity(arrayList), 16);
}
// endregion

// region ArrayList_clear Tests
Test(Collections, ArrayList_clear_Empty) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 0, nullptr);
    ArrayList_clear(arrayList);
    cr_assert_eq(arrayList->length, 0);
}

Test(Collections, ArrayList_clear_HasItems) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(arrayList, &i);
    }
    ArrayList_clear(arrayList);
    cr_assert_eq(arrayList->length, 0);
}
// endregion

// region ArrayList_resize Tests
Test(Collections, ArrayList_resize_ZeroToNonZeroCapacity) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 0, nullptr);
    ArrayList_resize(arrayList, 16);
    cr_assert_eq(arrayList->capacity, 16);
    cr_assert_not_null(arrayList->items);
}

Test(Collections, ArrayList_resize_NonZeroToZeroCapacity) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    ArrayList_resize(arrayList, 0);
    cr_assert_eq(arrayList->capacity, 0);
    cr_assert_null(arrayList->items);
}

Test(Collections, ArrayList_resize_NonZeroToNonZeroCapacity) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    const void* before = arrayList->items;
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(arrayList, &i);
    }
    ArrayList_resize(arrayList, 32);
    cr_assert_eq(arrayList->capacity, 32);
    cr_assert_neq(arrayList->items, before);
    for (int i = 0; i < 10; i++) {
        cr_assert_eq(*(int*)ArrayList_getItem(arrayList, i), i);
    }
}

Test(Collections, ArrayList_resize_FailedResizeMalloc) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    const void* before = arrayList->items;
    ArrayList_resize(arrayList, -1);
    cr_assert_eq(arrayList->capacity, 16);
    cr_assert_eq(arrayList->items, before);
}

Test(Collections, ArrayList_resize_LessThanLength) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    const void* before = arrayList->items;
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(arrayList, &i);
    }
    ArrayList_resize(arrayList, 8);
    cr_assert_eq(arrayList->capacity, 16);
    cr_assert_eq(arrayList->items, before);
    for (int i = 0; i < 10; i++) {
        cr_assert_eq(*(int*)ArrayList_getItem(arrayList, i), i);
    }

}
// endregion

// region ArrayList_getItem Tests
Test(Collections, ArrayList_getItem_NullList) {
    const int* item = ArrayList_getItem(nullptr, 0);
    cr_assert_null(item);
}

Test(Collections, ArrayList_getItem_IndexOOB) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(arrayList, &i);
    }
    const int* item = ArrayList_getItem(arrayList, 12);
    cr_assert_null(item);
}

Test(Collections, ArrayList_getItem_Success) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(arrayList, &i);
    }
    const int* item = ArrayList_getItem(arrayList, 4);
    cr_assert_not_null(item);
    cr_assert_eq(*item, 4);
}
// endregion

// region ArrayList_insertItem Tests
Test(Collections, ArrayList_insertItem_NullList) {
    int c = 1;
    const int* item = ArrayList_insertItem(nullptr, 0, &c);
    cr_assert_null(item);
}

Test(Collections, ArrayList_insertItem_NullItem) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 0, nullptr);
    const int* item = ArrayList_insertItem(arrayList, 0, nullptr);
    cr_assert_null(item);
}

Test(Collections, ArrayList_insertItem_OOB) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 0, nullptr);
    const int* item = ArrayList_insertItem(arrayList, 1, nullptr);
    cr_assert_null(item);
}

Test(Collections, ArrayList_insertItem_SuccessMiddle) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);

    // Insert elements into ArrayList
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(arrayList, &i);
    }
    const size_t lenBefore = arrayList->length;

    // Insert item into the middle
    int c = 1;
    const int* item = ArrayList_insertItem(arrayList, 5, &c);

    // Ensure elements before are correct
    for (int i = 0; i < 5; i++) {
        int* a = ArrayList_getItem(arrayList, i);
        cr_assert_eq(*a, i);
    }

    // Ensure elements after are correct
    for (int i = 6; i < 11; i++) {
        int* a = ArrayList_getItem(arrayList, i);
        cr_assert_eq(*a, i - 1);
    }

    // Ensure element was inserted correctly
    cr_assert_eq(arrayList->length, lenBefore + 1);
    cr_assert_not_null(item);
    cr_assert_neq(item, &c);
    cr_assert_eq(*item, c);
}

Test(Collections, ArrayList_insertItem_SuccessResize) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);

    // Insert elements into ArrayList
    for (int i = 0; i < 16; i++) {
        ArrayList_pushBackItem(arrayList, &i);
    }
    const size_t lenBefore = arrayList->length;
    const size_t capacityBefore = arrayList->capacity;

    // Insert item into the middle
    int c = 1;
    const int* item = ArrayList_insertItem(arrayList, 16, &c);

    // Ensure element was inserted correctly
    cr_assert_eq(arrayList->length, lenBefore + 1);
    cr_assert_eq(arrayList->capacity, capacityBefore * 2);
    cr_assert_not_null(item);
    cr_assert_neq(item, &c);
    cr_assert_eq(*item, c);
}
// endregion

// region ArrayList_removeItem Tests
Test(Collections, ArrayList_removeItem_NullList) {
    const int* item = ArrayList_removeItem(nullptr, 0);
    cr_assert_null(item);
}

Test(Collections, ArrayList_removeItem_Empty) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 0, nullptr);
    const int* item = ArrayList_removeItem(arrayList, 0);
    cr_assert_null(item);
}

Test(Collections, ArrayList_removeItem_OOB) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(arrayList, &i);
    }
    const int* item = ArrayList_removeItem(arrayList, 11);
    cr_assert_null(item);
}

Test(Collections, ArrayList_removeItem_Success) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);

    // Insert elements into ArrayList
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(arrayList, &i);
    }
    const size_t lenBefore = arrayList->length;

    // Get item from the middle
    const int* itemBefore = ArrayList_getItem(arrayList, 5);
    const int valueBefore = *itemBefore;

    // Remove item from the middle
    int c = 1;
    const int* item = ArrayList_removeItem(arrayList, 5);

    // Ensure remaining elements are correct
    const int expected[9] = { 0, 1, 2, 3, 4, 6, 7, 8, 9 };
    cr_assert_arr_eq(arrayList->items, expected, 9);

    // Ensure element was removed correctly
    cr_assert_eq(arrayList->length, lenBefore - 1);
    cr_assert_not_null(item);
    cr_assert_neq(item, itemBefore);
    cr_assert_eq(*item, valueBefore);
}
// endregion

// region ArrayList_destroyItem Tests
Test(Collections, ArrayList_destroyItem_Empty) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 0, nullptr);
    ArrayList_destroyItem(arrayList, 0);
    cr_assert_eq(arrayList->length, 0);
}

Test(Collections, ArrayList_destroyItem_OOB) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);
    const ArrayList stateBefore = *arrayList;

    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(arrayList, &i);
    }
    const int* item = ArrayList_removeItem(arrayList, 11);

    cr_assert_null(item);
    cr_assert_arr_eq(arrayList, &stateBefore, 1);
}

Test(Collections, ArrayList_destroyItem_Success) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);

    // Insert elements into ArrayList
    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(arrayList, &i);
    }
    const size_t lenBefore = arrayList->length;

    // Remove item from the middle
    ArrayList_destroyItem(arrayList, 5);

    // Ensure remaining elements are correct
    const int expected[9] = { 0, 1, 2, 3, 4, 6, 7, 8, 9 };
    cr_assert_arr_eq(arrayList->items, expected, 9);

    // Ensure element was removed correctly
    cr_assert_eq(arrayList->length, lenBefore - 1);
}
// endregion

// region ArrayList_pushBackItem Tests
Test(Collections, ArrayList_pushBackItem_NullList) {
    int c = 0;
    const void* item = ArrayList_pushBackItem(nullptr, &c);
    cr_assert_null(item);
}

Test(Collections, ArrayList_pushBackItem_Success) {
    ArrayList* arrayList = ArrayList_create(sizeof(int), 16, nullptr);

    int c = 0;
    const int* item = ArrayList_pushBackItem(arrayList, &c);

    cr_assert_eq(arrayList->length, 1);
    cr_assert_not_null(item);
    cr_assert_neq(item, &c);
    cr_assert_eq(*item, c);
}
// endregion

// region ArrayList_forEach
static int sum;
static void addItem(void* item) {
    sum += *(int*)item;
}

Test(Collections, ArrayList_forEach_NullList) {
    ArrayList_forEach(nullptr, addItem);
    cr_assert(true);
}

Test(Collections, ArrayList_forEach_NullAction) {
    ArrayList* list = ArrayList_create(sizeof(int), 8, nullptr);
    ArrayList_forEach(list, nullptr);
    cr_assert(true);
}

Test(Collections, ArrayList_forEach_Success) {
    sum = 0;
    ArrayList* list = ArrayList_create(sizeof(int), 8, nullptr);

    for (int i = 1; i <= 5; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    ArrayList_forEach(list, addItem);
    cr_assert_eq(sum, 15);
}
// endregion

// region ArrayList_find
static int compareInt(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

Test(Collections, ArrayList_find_NullList) {
    int key = 5;
    cr_assert_null(ArrayList_find(nullptr, &key, compareInt));
}

Test(Collections, ArrayList_find_NullCompare) {
    ArrayList* list = ArrayList_create(sizeof(int), 8, nullptr);
    int key = 5;
    cr_assert_null(ArrayList_find(list, &key, nullptr));
}

Test(Collections, ArrayList_find_NotFound) {
    ArrayList* list = ArrayList_create(sizeof(int), 8, nullptr);
    const int key = 5;
    cr_assert_null(ArrayList_find(list, &key, compareInt));
}

Test(Collections, ArrayList_find_Success) {
    ArrayList* list = ArrayList_create(sizeof(int), 8, nullptr);

    for (int i = 0; i < 10; i++) {
        ArrayList_pushBackItem(list, &i);
    }

    const int key = 6;
    int* value = ArrayList_find(list, &key, compareInt);

    cr_assert_not_null(value);
    cr_assert_eq(*value, 6);
}
// endRegion