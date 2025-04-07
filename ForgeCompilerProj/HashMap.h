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
    unsigned long (*hash_func)(void*, int mapSize); // generic hash_func
    int (*equal_func)(void*, void*); // generic equals function
    void (*free_key)(void*);         // generic free func for the key
    void (*FreeValue)(void*);       // generic free func for the value
    void (*print_key)(void*);        // generic print function for printing key
    void (*print_value)(void*);      // generic print function for printing a value
} HashMap;  

// this func will create an empty hashMap and set the hash and equals func for its items
HashMap* init_hashmap(int size, unsigned long (*hash_func)(void*, int), int (*equal_func)(void*, void*),
    void (*print_key)(void*), void (*print_value)(void*), void (*free_key)(void*), void (*FreeValue)(void*));
// this func will free a given hash map
void free_hashmap(HashMap** map);
// this func will get the hashmap value based on a key
void* get_hashmap_value(void* key, HashMap* map);
// this func will insert a new value into the hashmap
void insert_new_value(void* key, void* value, HashMap* map);
// this func will create a HashNode with the vars it needs
HashNode* get_hash_node(void* key, void* value);
// this func will resize the hash map if needed
void resize_map(HashMap* map);
// this func will print the contents of the hashmap using function pointers 
void print_hashmap(HashMap* map);
// this func will return a hash for string values
unsigned long djb2Hash(const char* str);

#endif
