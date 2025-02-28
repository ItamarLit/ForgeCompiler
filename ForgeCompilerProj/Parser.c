#pragma warning (disable:4996)
#include "Parser.h"
#include "FileReader.h"
#include "HashMap.h"
#include "Terminals.h"
#include "NonTerminals.h"
#include <string.h>
#include <stdio.h>  
#include <stdlib.h>

void freeKey(void* key);
void GetData(char** dataArr, char* line, int isAction);
unsigned long djb2Hash(const char* str);
unsigned long hashFunc(void* key, int map_size);
void FillTable(HashMap** map, char* filename, char** symbolArray, int symbolCount, int isAction, void (*PutRow)(int state, const char* symbol, const char* value, HashMap* map));
static int equalFunc(void* a, void* b);
static void printKey(void* key);


MapKey* getKey(int state, char* symbol) {
    // allocate key
    MapKey* key = malloc(sizeof(MapKey));
    if (!key) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    key->currentState = state;
    key->symbol = strdup(symbol);
    return key;
}

/// <summary>
/// This func puts a new state in the action hashmap
/// </summary>
/// <param name="state"></param>
/// <param name="terminal"></param>
/// <param name="action"></param>
/// <param name="map"></param>
static void actionPutState(int state,const char* terminal, const char* action, HashMap* map) {
    // allocate key
    MapKey* key = getKey(state, terminal);
    // allocate the value
    char* val = strdup(action);
    // insert the key - val pair
    insertNewValue(key, val, map);
}

static void actionPrintValue(void* value) {
    char* fvalue = (char*)value;
    printf("Value: action: %s] ", fvalue);
}

/// <summary>
/// This func will setup the action table hashmap
/// </summary>
/// <param name="map"></param>
/// <param name="filename"></param>
void InitActionTable(HashMap** map, char* filename)
{
    *map = initHashMap(INITAL_HASHMAP_SIZE, hashFunc, equalFunc, printKey, actionPrintValue, freeKey, free);
    FillTable(map, filename, Terminals, TerminalCount, 1, actionPutState);
}



static void gotoPrintValue(void* value) {
    int* fvalue = (int*)value;
    printf("Value: nextState: %d] ", *fvalue);
}

static void gotoPutState(int state, const char* nonTerminal, const char* nextState, HashMap* map) {
    MapKey* key = getKey(state, nonTerminal);
    // allocate the value
    int* val = malloc(sizeof(int));
    *val = atoi(nextState);
    // insert the key - val pair
    insertNewValue(key, val, map);
}


void InitGotoTable(HashMap** map, char* filename) {
    *map = initHashMap(INITAL_HASHMAP_SIZE, hashFunc, equalFunc, printKey, gotoPrintValue, freeKey, free);
    FillTable(map, filename, NonTerminals, NonTerminalCount, 0, gotoPutState);
}

void freeKey(void* key)
{
    MapKey* fkey = (MapKey*)key;
    free(fkey->symbol);
    free(fkey);
}

/// <summary>
/// This func will get the data from the csv file line and fill the dataArr, if there is no action / goto then the arr[i] is empty
/// </summary>
/// <param name="dataArr"></param>
/// <param name="line"></param>
/// <param name="isAction"></param>
void GetData(char** dataArr, char* line, int isAction)
{
    int maxData = isAction ? TerminalCount : NonTerminalCount;
    int count = 0;
    char* start = line;
    char* end = NULL;

    while (count < maxData)
    {
        // find the next ,
        end = strchr(start, ',');
        if (end == NULL)
        {
            // last token
            dataArr[count] = strdup(start);
            count++;
        }
        else
        {
            // get the token data
            int length = (int)(end - start);
            char* token = (char*)malloc(length + 1);
            if (!token) {
                printf("Memory allocation failed\n");
            }
            memcpy(token, start, length);
            token[length] = '\0';
            dataArr[count] = token;
            count++;
            // move the start
            start = end + 1;
        }
    }
    // if the dataArr isnt filled yet, fill with empty strings
    while (count < maxData)
    {
        dataArr[count++] = strdup("");
    }
}

/// <summary>
/// This is the hash func for the action string, it is a djb2 hash func
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
unsigned long djb2Hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/// <summary>
/// This is the hash func for the action table
/// </summary>
/// <param name="key"></param>
/// <param name="map_size"></param>
/// <returns></returns>
unsigned long hashFunc(void* key, int map_size) {
    MapKey* fkey = (MapKey*)key;
    unsigned long terminalHash = djb2Hash(fkey->symbol);
    return (terminalHash + (fkey->currentState * 31)) % map_size;
}


void FillTable(HashMap** map, char* filename, char** symbolArray, int symbolCount, int isAction,
    void (*PutRow)(int state, const char* symbol, const char* value, HashMap* map))
{
    char* fileData = readFile(filename);
    char* saveptr1;
    char* line = strtok_s(fileData, "\n", &saveptr1);
    int stateCount = 0;
    while (line != NULL) {
        // skip empty lines
        if (strlen(line) == 0) {
            line = strtok_s(NULL, "\n", &saveptr1);
            continue;
        }

        // get the line data
        char** tempData = (char**)malloc(symbolCount * sizeof(char*));
        char* lineCopy = strdup(line);
        GetData(tempData, lineCopy, isAction);
        free(lineCopy);
        for (int i = 0; i < symbolCount; i++) {
            if (strcmp(tempData[i], "") != 0) {
                PutRow(stateCount, symbolArray[i], tempData[i], *map);
            }
            // free the duped str
            free(tempData[i]);
        }
        // free the str array
        free(tempData);
        stateCount++;
        // move to the next line
        line = strtok_s(NULL, "\n", &saveptr1);
    }
    // free the buffer
    free(fileData);
}

static int equalFunc(void* a, void* b) {
    MapKey* k1 = (MapKey*)a;
    MapKey* k2 = (MapKey*)b;
    return (k1->currentState == k2->currentState) &&
        (strcmp(k1->symbol, k2->symbol) == 0);
}

static void printKey(void* key)
{
    MapKey* fkey = (MapKey*)key;
    printf("[Key: current_state: %d, symbol: '%s' ", fkey->currentState, fkey->symbol);
}