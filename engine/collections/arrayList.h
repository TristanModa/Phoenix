#ifndef ENGINE_COLLECTIONS_ARRAYLIST_H
#define ENGINE_COLLECTIONS_ARRAYLIST_H

#include <cc_array.h>

#include "common.h"

typedef struct arrayList {
	CC_Array* cc_array;
	size_t itemSize;
	ItemDestructorFn itemDestructor;
} ArrayList;

typedef struct arrayListIter {
	CC_ArrayIter cc_arrayIter;
	ArrayList* arrayList;
} ArrayListIter;

ArrayList* ArrayList_create(size_t itemSize, ItemDestructorFn itemDestructor);
void ArrayList_destroy(ArrayList* arrayList);

size_t ArrayList_getSize(const ArrayList* arrayList);

void ArrayList_clear(ArrayList* arrayList);
void ArrayList_trimCapacity(ArrayList* arrayList);

void* ArrayList_getItem(const ArrayList* arrayList, size_t index);
void* ArrayList_getBackItem(const ArrayList* arrayList);

void* ArrayList_insertItem(ArrayList* arrayList, size_t index, const void* item);
void ArrayList_removeItem(ArrayList* arrayList, size_t index, void* out);
void* ArrayList_replaceItem(ArrayList* arrayList, size_t index, const void* item, void* out);

void* ArrayList_pushBackItem(ArrayList* arrayList, const void* item);
void ArrayList_popBackItem(ArrayList* arrayList, void* out);

void ArrayList_forEach(ArrayList* arrayList, ItemActionFn action);
void* ArrayList_find(const ArrayList* arrayList, const void* key, ItemCompareFn compare);
void ArrayList_sort(ArrayList* arrayList, ItemCompareFn compare);

ArrayListIter ArrayList_begin(ArrayList* arrayList);

void* ArrayListIter_getNext(ArrayListIter* it);

size_t ArrayListIter_getIndex(ArrayListIter* it);

void* ArrayListIter_insertItem(ArrayListIter* it, const void* item);
void ArrayListIter_removeItem(ArrayListIter* it, void* out);
void* ArrayListIter_replaceItem(ArrayListIter* it, const void* item, void* out);

#endif //ENGINE_COLLECTIONS_ARRAYLIST_H