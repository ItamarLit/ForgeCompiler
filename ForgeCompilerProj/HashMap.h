#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>
#define INITAL_HASHMAP_SIZE 400

typedef struct HashNode {
    void* key;                   // generic key
    void* value;                 // generic value
    struct HashNode* next;       // next node in linked list
} HashNode;

typedef struct HashMap {
    HashNode** arr;              // array of nodes
    int map_size;                // size of arr
    int usedSpaces;              // counter for used spaces
    float load_factor;           // load factor used for resizing 
    unsigned long (*HashFunc)(void*, int mapSize); // generic hashfunc
    int (*EqualFunc)(void*, void*); // generic equals function
    void (*FreeKey)(void*);         // generic free func for the key
    void (*FreeValue)(void*);       // generic free func for the value
    void (*PrintKey)(void*);        // generic print function for printing key
    void (*PrintValue)(void*);      // generic print function for printing a value
} HashMap;  

// this func will create an empty hashMap and set the hash and equals func for its items
HashMap* initHashMap(int size, unsigned long (*HashFunc)(void*, int), int (*EqualFunc)(void*, void*),
    void (*PrintKey)(void*), void (*PrintValue)(void*), void (*FreeKey)(void*), void (*FreeValue)(void*));
// this func will free a given hash map
void freeHashMap(HashMap** map);
// this func will get the hashmap value based on a key
void* getHashMapValue(void* key, HashMap* map);
// this func will insert a new value into the hashmap
void insertNewValue(void* key, void* value, HashMap* map);
// this func will create a HashNode with the vars it needs
HashNode* getHashNode(void* key, void* value);
// this func will resize the hash map if needed
void resizeMap(HashMap* map);
// this func will print the contents of the hashmap using function pointers 
void printHashMap(HashMap* map);
#endif
