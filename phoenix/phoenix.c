#include "phoenix.h"

#include <vulfEngine.h>

void printHashMap(const HashMap* hashMap);

typedef struct person {
	const char* name;
	int age;
} Person;

void Phoenix_init() {
	HashMap* hashMap = HashMap_create(4, sizeof(Person));
	printHashMap(hashMap);

	//
	Person person = { .name = "Edmund", .age = 20 };
	HashMap_insert(hashMap, person.name, &person);
	person = (Person){ .name = "Brandon", .age = 23 };
	HashMap_insert(hashMap, person.name, &person);
	person = (Person){ .name = "Shibby", .age = 22 };
	HashMap_insert(hashMap, person.name, &person);
	person = (Person){ .name = "Nelson", .age = 21 };
	HashMap_insert(hashMap, person.name, &person);

	printHashMap(hashMap);

	HashMap_remove(hashMap, "Nelson");

	printHashMap(hashMap);

	HashMap_remove(hashMap, "Edmund");
	person = (Person){ .name = "Nelson", .age = 21 };
	HashMap_insert(hashMap, person.name, &person);

	printHashMap(hashMap);


}

void Phoenix_destroy() {
}

void Phoenix_update() {
}

void Phoenix_tick() {

}

void Phoenix_render() {

}

void printHashMap(const HashMap* hashMap) {
	printf("Table\n");
	for (size_t i = 0; i < hashMap->bucketCount; i++) {
		printf("%zu - ", i);
		LinkedList* bucket = hashMap->buckets[i];
		if (!bucket) {
			printf("--------\n");
		} else {
			for (size_t j = 0; j < bucket->length; j++) {
				BucketEntry* bucketEntry = LinkedList_getItem(bucket, j);
				Person* person = bucketEntry->item;
				printf("%s (%d), ", person->name, person->age);
			}
			printf("\n");
		}
	}
	printf("\n");
}