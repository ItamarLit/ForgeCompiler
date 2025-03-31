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
    unsigned long hash = 5381;
    int c;
    while ((c = *fkey++)) {
        hash = ((hash << 5) + hash) + c;
    }
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
        printf("Unable to malloc string entry for string table");
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

static void createStringTableRecursive(ASTNode* node, HashMap* map)
{
    if (!node) return;

    if (node->lable && strcmp(node->lable, "VarDeclaration") == 0) {
        ASTNode* name = node->children[1];
        SymbolTable* scope = getClosestScope(node);
        SymbolEntry* entry = lookUpSymbol(name->token->lexeme, scope);
        // found local string with no entry
        if (entry->place == IS_LOCAL && entry->type == TYPE_STRING && !getMapValue(map, node->children[3]->token->lexeme))
        {
            insertString(map, node->children[3]->token->lexeme);
        }
    }
    if (node->lable && strcmp(node->lable, "AssignmentStatement") == 0 ) {
        ASTNode* name = node->children[0];
        SymbolTable* scope = getClosestScope(node);
        SymbolEntry* entry = lookUpSymbol(name->token->lexeme, scope);
        if (entry->type == TYPE_STRING) {
            ASTNode* string = node->children[2];
            // insert every string
            if (!getMapValue(map, string->token->lexeme))
            {
                insertString(map, string->token->lexeme);
            }
        }
    }
    for (int i = 0; i < node->childCount; i++) {
        createStringTableRecursive(node->children[i], map);
    }
}


HashMap* createStringTable(ASTNode* root)
{
    HashMap* map = initHashMap(INITAL_HASHMAP_SIZE, hashFunc, equalFunc, printStringKey, printSymbolEntry, free, freeStringEntry);
    if (!map) {
        printf("Failed to create string table hashmap.\n");
        return NULL;
    }
    createStringTableRecursive(root, map);
    return map;
}

char* lookUpString(char* string, HashMap* map) 
{
    StringEntry* entry = getMapValue(map, string);
    if (!entry) return NULL;
    return entry->label;
}
