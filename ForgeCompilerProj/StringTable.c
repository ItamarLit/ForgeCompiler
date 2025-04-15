#pragma warning (disable:4996)
#include "StringTable.h"
#include "HashMap.h"
#include "SymbolTable.h"
#include "ErrorHandler.h"
#include <string.h>

/// <summary>
/// hash func for strings
/// </summary>
/// <param name="key"></param>
/// <param name="map_size"></param>
/// <returns>Returns an index in the hashmap</returns>
static unsigned long hash_func(void* key, int map_size) {
    char* fkey = (char*)key;
    unsigned long hash = djb2Hash(fkey);
    return hash % map_size;
}

/// <summary>
/// This is a helper func that gens a unique str name that refrences the read only string
/// </summary>
/// <returns></returns>
char* gen_str_label() 
{
    static int labelNum = 0;
    // static so pointer is valid after the function returns
	static char label[32];  
	sprintf(label, "_str%d", labelNum++);
	return label;
}

/// <summary>
/// This function inserts a string into the string table
/// </summary>
/// <param name="map"></param>
/// <param name="string"></param>
void insertString(HashMap* map, char* string) {
    StringEntry* entry = (StringEntry*)malloc(sizeof(StringEntry));
    if (!entry) {
        output_error(GENERAL, "Unable to malloc string entry for string table\n");
        return;
    }
    entry->label = strdup(gen_str_label());
    // remove 2 for the "
    entry->originalLen = strlen(string) - 2;
    // insert value in hashmap
    insert_new_value(strdup(string), entry, map);
}

/// <summary>
/// This func returns the value 
/// </summary>
/// <param name="map"></param>
/// <param name="string"></param>
/// <returns>Returns String entry from hashmap if found</returns>
static StringEntry* get_map_value(HashMap* map, char* string) {
    return (StringEntry*)get_hashmap_value(string, map);
}

/// <summary>
/// Function that frees a string entry
/// </summary>
/// <param name="value"></param>
void freeStringEntry(void* value) {
    StringEntry* entry = (StringEntry*)value;
    free(entry->label);
    free(entry);
}

/// <summary>
/// Equal func
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns>Returns true / false if equal</returns>
static int equal_func(void* a, void* b) {
    char* k1 = (char*)a;
    char* k2 = (char*)b;
    return strcmp(k1, k2) == 0;
}

// Print functions
static void print_string_key(void* key) {
    printf("[Key: string: %s ", (char*)key);
}

static void print_symbol_entry(void* value) {
    printf("Value: %s ]", ((StringEntry*)value)->label);
}

/// <summary>
/// This func will save all the string literals in a program in the readonly segment
/// </summary>
/// <param name="node"></param>
/// <param name="map"></param>
static void create_string_tableRecursive(ASTNode* node, HashMap* map)
{
    if (!node) return;
    // Save all global string literals that are in the program and not saved yet
    if (node->token && node->token->type == STRING_LITERAL && !get_map_value(map, node->token->lexeme))
    {
        insertString(map, node->token->lexeme);
    }
    // go over all the children
    for (int i = 0; i < node->childCount; i++) {
        create_string_tableRecursive(node->children[i], map);
    }
}

/// <summary>
/// This function creates a full string table
/// </summary>
/// <param name="root"></param>
/// <returns></returns>
HashMap* create_string_table(ASTNode* root)
{
    HashMap* map = init_hashmap(INITAL_HASHMAP_SIZE, hash_func, equal_func, print_string_key, print_symbol_entry, free, freeStringEntry);
    if (!map) {
        output_error(GENERAL, "Failed to create string table hashmap\n");
        return NULL;
    }
    create_string_tableRecursive(root, map);
    return map;
}

/// <summary>
/// This function is used to lookup string values and get there labels if they exist
/// </summary>
/// <param name="string"></param>
/// <param name="map"></param>
/// <returns></returns>
char* look_up_string(char* string, HashMap* map) 
{
    StringEntry* entry = get_map_value(map, string);
    if (!entry) return NULL;
    return entry->label;
}
