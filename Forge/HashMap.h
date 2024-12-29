#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>
#define INITAL_HASHMAP_SIZE 400

typedef struct HashNode {
    int current_state;           // current state in FSM
    char current_char;           // Input character
    int next_state;              // next state in FSM
    struct HashNode* next;       // next node in linked list
} HashNode;

typedef struct HashMap {
    HashNode** arr;              // array of nodes
    int map_size;                // size of arr
    int usedSpaces;              // counter for used spaces
    float load_factor;           // load factor used for resizing 
} HashMap;

// this func will create an empty hashMap
HashMap* initHashMap(int size);
// this func will free a given hash map
void freeHashMap(HashMap** map);
// this func will generate a hashcode for a given state, char pair
int getHashCode(int currentState, char currentChar, HashMap* map);
// this func will get the next state based on the current state and char
int getNextState(int currentState, char currentChar, HashMap* map);
// this func will put info in an index in the hash map
void putState(int currentState, char currentChar, int nextState, HashMap* map);
// this func will create a HashNode with the vars it needs
HashNode* getHashNode(int current_state, char current_char, int next_state);
// this func will resize the hash map if needed
void resizeMap(HashMap* map);
// print func used for debugging
void printHashMap(HashMap* map);


#endif
