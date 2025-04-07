#pragma warning (disable:4996)
#include "StringTable.h"
#include "HashMap.h"
#include "SymbolTable.h"
#include <string.h>

/// <summary>
/// hash func for strings
/// </summary>
/// <param name="key"></param>
/// <param name="map_size"></param>
/// <returns>Returns an index in the hashmap</returns>
static unsigned long hashFunc(void* key, int map_size) {
    char* fkey = (char*)key;
    unsigned long hash = djb2Hash(fkey);
    return hash % map_size;
}

/// <summary>
/// This is a helper func that gens a unique str name that refrences the read only string
/// </summary>
/// <returns></returns>
char* genStrLabel() 
{
    static int labelNum = 0;
    // static so pointer is valid after the function returns
	static char label[32];  
	sprintf(label, "_str%d", labelNum++);
	return label;
}


void insertString(HashMap* map, char* string) {
    StringEntry* entry = (StringEntry*)malloc(sizeof(StringEntry));
    if (!entry) {
        fprintf(stderr, "Unable to malloc string entry for string table\n");
        return;
    }
    entry->label = strdup(genStrLabel());
    // remove 2 for the "
    entry->originalLen = strlen(string) - 2;
    // insert value in hashmap
    insertNewValue(strdup(string), entry, map);
}

static StringEntry* getMapValue(HashMap* map, char* string) {
    return (StringEntry*)getHashMapValue(string, map);
}

void freeStringEntry(void* value) {
    StringEntry* entry = (StringEntry*)value;
    free(entry->label);
    free(entry);
}

static int equalFunc(void* a, void* b) {
    char* k1 = (char*)a;
    char* k2 = (char*)b;
    return strcmp(k1, k2) == 0;
}

static void printStringKey(void* key) {
    printf("[Key: string: %s ", (char*)key);
}

static void printSymbolEntry(void* value) {
    printf("Value: %s ]", ((StringEntry*)value)->label);
}

/// <summary>
/// This func will save all the string literals in a program in the readonly segment
/// </summary>
/// <param name="node"></param>
/// <param name="map"></param>
static void createStringTableRecursive(ASTNode* node, HashMap* map)
{
    if (!node) return;
    // Save all global string literals that are in the program and not saved yet
    if (node->token && node->token->type == STRING_LITERAL && !getMapValue(map, node->token->lexeme))
    {
        insertString(map, node->token->lexeme);
    }
    // go over all the children
    for (int i = 0; i < node->childCount; i++) {
        createStringTableRecursive(node->children[i], map);
    }
}

/// <summary>
/// This function creates a full string table
/// </summary>
/// <param name="root"></param>
/// <returns></returns>
HashMap* createStringTable(ASTNode* root)
{
    HashMap* map = initHashMap(INITAL_HASHMAP_SIZE, hashFunc, equalFunc, printStringKey, printSymbolEntry, free, freeStringEntry);
    if (!map) {
        fprintf(stderr, "Failed to create string table hashmap.\n");
        return NULL;
    }
    createStringTableRecursive(root, map);
    return map;
}

/// <summary>
/// This function is used to lookup string values and get there labels if they exist
/// </summary>
/// <param name="string"></param>
/// <param name="map"></param>
/// <returns></returns>
char* lookUpString(char* string, HashMap* map) 
{
    StringEntry* entry = getMapValue(map, string);
    if (!entry) return NULL;
    return entry->label;
}
