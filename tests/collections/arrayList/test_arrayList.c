#include <criterion/criterion.h>
#include <collections/arrayList.h>

#include "core/memory.h"

void setup() {
    const MemoryProperties properties = {
        .malloc = malloc,
        .calloc = calloc,
        .realloc = realloc,
        .free = free,
        .compare = memcmp,
        .copy = memcpy,
        .move = memmove
    };
    Memory_setProperties(&properties);
    Memory_init();
}

Test(arrayList_suite, test_create, .init = setup) {
    const ArrayList* arrayList = ArrayList_create(sizeof(int), 0);
    cr_assert_not_null(arrayList);
}