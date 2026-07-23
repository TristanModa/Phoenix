#include "phoenix.h"

#include <stdlib.h>
#include <vulfEngine.h>

typedef struct person {
    const char* name;
    int age;
} Person;

void printPerson(void* kvp) {
    Person* p = ((KeyValuePair*)kvp)->value;
    Logger_info("%s %d", p->name, p->age);
    fflush(stdout);
}

void Phoenix_init() {
    // Create the HashMap
    HashMap* hashMap = HashMap_create(80, sizeof(Person), nullptr);

    // Populate HashMap
    for (size_t i = 0; i < 26; i++) {
        char c[2] = {
            (char)('A' + (int)i),
            '\0'
        };
        Person p = { .name = c, .age = (int)i };
        HashMap_insertItem(hashMap, p.name, &p);
    }

    // Print the contents of the HashMap
    HashMap_forEach(hashMap, printPerson);
}

void Phoenix_destroy() {
}

void Phoenix_update() {
}

void Phoenix_tick() {

}

void Phoenix_render() {

}