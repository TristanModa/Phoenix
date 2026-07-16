#ifndef ENGINE_COLLECTIONS_ARRAYLIST_H
#define ENGINE_COLLECTIONS_ARRAYLIST_H

#include <stddef.h>

typedef struct arrayList {
	size_t length;
	size_t capacity;
	size_t itemSize;
	void* items;
} ArrayList;

ArrayList* ArrayList_create(size_t itemSize, size_t capacity);
void ArrayList_destroy(ArrayList* arrayList);

void ArrayList_clear(ArrayList* arrayList);
void ArrayList_resize(ArrayList* arrayList, size_t capacity);

size_t ArrayList_getLength(const ArrayList* arrayList);
size_t ArrayList_getCapacity(const ArrayList* arrayList);

void* ArrayList_get(const ArrayList* arrayList, size_t index);
void* ArrayList_getFirst(const ArrayList* arrayList);
void* ArrayList_getLast(const ArrayList* arrayList);
void ArrayList_set(const ArrayList* arrayList, size_t index, const void* item);

const void* ArrayList_getItems(const ArrayList* arrayList);

void ArrayList_pushBack(ArrayList* arrayList, const void* item);
void ArrayList_popBack(ArrayList* arrayList);

void ArrayList_insert(ArrayList* arrayList, const void* item, size_t index);
void ArrayList_remove(ArrayList* arrayList, size_t index);

size_t ArrayList_indexOf(const ArrayList* arrayList, const void* item);

#endif //ENGINE_COLLECTIONS_ARRAYLIST_H
