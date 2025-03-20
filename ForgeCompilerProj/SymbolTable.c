#pragma warning (disable:4996)
#include "SymbolTable.h"
#include "AST.h"
#include <string.h>
#include <stdio.h>


/// <summary>
/// Hash func for the symbol table, each key is the symbol name
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

static void putState(HashMap* map, char* name, char* type, int scope, int isFunction, char* returnType, char** paramTypes, int paramCount, int line) {
    if (getHashMapValue(name, map) != NULL) {
        printf("Error: Identifier '%s' already declared in this scope!\n", name);
        return;
    }
    // Allocate and fill SymbolEntry struct
    SymbolEntry* entry = malloc(sizeof(SymbolEntry));
    if (!entry) {
        printf("Failed to malloc entry for symbol table");
        return;
    }
    entry->name = strdup(name);
    entry->type = strdup(type);
    entry->line = line;
    entry->isFunction = isFunction;
    entry->returnType = returnType ? strdup(returnType) : NULL;
    entry->paramTypes = (char**)malloc(sizeof(char*) * paramCount);
    for (int i = 0; i < paramCount; i++) {
        entry->paramTypes[i] = strdup(paramTypes[i]);
    }
    entry->paramCount = paramCount;
    // insert value in hashmap
    insertNewValue(strdup(name), entry, map);
}

static SymbolEntry* getMapValue(HashMap* map, char* name) {
    return (SymbolEntry*)getHashMapValue(name, map);
}

void freeSymbolEntry(void* value) {
    SymbolEntry* entry = (SymbolEntry*)value;
    free(entry->name);
    free(entry->type);
    if (entry->returnType) free(entry->returnType);
    for (int i = 0; i < entry->paramCount; i++) {
        free(entry->paramTypes[i]);
    }
    free(entry->paramTypes);
    free(entry);
}

static int equalFunc(void* a, void* b) {
    char* k1 = (char*)a;
    char* k2 = (char*)b;
    return strcmp(k1, k2) == 0;
}

void printStringKey(void* key) {
    printf("[Key: symbol: %s ", (char*)key);
}

void printSymbolEntry(void* value) {
    SymbolEntry* entry = (SymbolEntry*)value;
    printf(" Value: Name: %s, Type: %s, Function: %d, ParamCount: %d, ParamTypes: ",
        entry->name, entry->type, entry->isFunction, entry->paramCount);
    if (entry->paramCount == 0) {
        printf("No params ]");
        return;
    }
    for (int i = 0; i < entry->paramCount - 1; i++) {
        printf("%s, ", entry->paramTypes[i]);
    }
    printf("%s ]", entry->paramTypes[entry->paramCount - 1]);
}

int extractFunctionParameters(ASTNode* paramNode, char*** paramNames, char*** paramTypes) {
    // get children count
    int childrenCount = paramNode->childCount;
    if (childrenCount == 0) {
        printf("Function has an empty parameter list.\n");
        return 0;
    }
    int decCount = 0;
    // go over all children of the param list
    for (int i = 0; i < childrenCount; i++) {
        ASTNode* paramDecl = paramNode->children[i];
        // get the type and identifier from the ParamDecl nodes
        if (strcmp(paramDecl->lable, "ParamDecl") == 0) {
            // realoc the arrays
            *paramNames = (char**)realloc(*paramNames, sizeof(char*) * (decCount + 1));
            *paramTypes = (char**)realloc(*paramTypes, sizeof(char*) * (decCount + 1));
            ASTNode* typeNode = paramDecl->children[0];
            (*paramTypes)[decCount] = strdup(typeNode->token->lexeme);
            ASTNode* nameNode = paramDecl->children[1];
            (*paramNames)[decCount] = strdup(nameNode->token->lexeme);
            decCount++;
        }
    }
    return decCount;
}


SymbolTable* createNewScope(SymbolTable* parent) {
    SymbolTable* newTable = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!newTable) {
        printf("Unable to malloc memory for symbol table");
        return NULL;
    }
    newTable->table = initHashMap(INITAL_HASHMAP_SIZE, hashFunc, equalFunc, printStringKey, printSymbolEntry, free, freeSymbolEntry);
    newTable->parent = parent;  // link the current table to its parent
    return newTable;
}



void createASTSymbolTable(ASTNode* node, SymbolTable* currentTable, int* errorCount) {
    if (!node) return;

    if (strcmp(node->lable, "VarDeclaration") == 0) {
        char* varName = node->children[1]->token->lexeme;  // Get variable name
        char* varType = node->children[0]->token->lexeme;  // Get variable type
        if (getMapValue(currentTable->table, varName)) {
            printf("Error: Variable %s already declared in this scope\n", varName);
            (*errorCount)++;
        }
        else {
            // insert the symbol
            putState(currentTable->table, varName, varType, 0, 0, NULL, NULL, 0, node->children[1]->token->tokenRow);
        }
    }

    else if (strcmp(node->lable, "FuncDeclaration") == 0) {
        char* funcName = node->children[0]->token->lexeme; // Function name
        ASTNode* paramList = node->children[1]; // paramListNode, exists even if no params
        char* returnType = node->children[2]->token->lexeme; // Return type
        if (getMapValue(currentTable->table, funcName)) {
            printf("Error: Function %s already declared\n", funcName);
            (*errorCount)++;
        }
        // go over all param dec in func 
        char** paramNames = NULL;
        char** paramTypes = NULL;
        int decCount = extractFunctionParameters(paramList, &paramNames, &paramTypes);
        putState(currentTable->table, funcName, returnType, 0, 1, returnType, paramTypes, decCount, IGNORE_LINE);
        
        // create new symbol table for func scope
        SymbolTable* functionScope = createNewScope(currentTable);
        node->scope = functionScope;      
        // fill the function scope with the params
        for (int i = 0; i < decCount; i++) {
            putState(functionScope->table, paramNames[i], paramTypes[i], 0, 0, NULL, NULL, 0, IGNORE_LINE);
            free(paramNames[i]);
            free(paramTypes[i]);
        }
        if (paramNames) {
            free(paramNames);
        }
        if (paramTypes) {
            free(paramTypes);
        }
        // go over function body
        createASTSymbolTable(node->children[7], functionScope, errorCount);
    }
    else if (strcmp(node->lable, "Block") == 0) {
        // Create a new symbol table for this block
        SymbolTable* blockScope = createNewScope(currentTable);
        node->scope = blockScope;
        // Traverse statements inside the block
        for (int i = 0; i < node->childCount; i++) {
            createASTSymbolTable(node->children[i], blockScope, errorCount);
        }
    }
    else {
        // Recursively traverse children
        for (int i = 0; i < node->childCount; i++) {
            createASTSymbolTable(node->children[i], currentTable, errorCount);
        }
    }
}


void printSymbolTables(ASTNode* node) {
    if (node == NULL) {
        return;
    }
    // check if there is a symbol table
    if (node->scope) {
        printf("Symbol Table for AST Node: %s\n", node->lable ? node->lable : "Unnamed Node");
        printHashMap(node->scope->table);
        printf("\n");  
    }
    // call for all children
    for (int i = 0; i < node->childCount; i++) {
        printSymbolTables(node->children[i]);
    }
}

/// <summary>
/// This func is used to lookup a symbol in a symbol table while traversing the AST
/// </summary>
/// <param name="symbol"></param>
/// <param name="currentScope"></param>
/// <returns>Returns the SymbolTable entry or NULL if it wasnt found</returns>
SymbolEntry* lookUpSymbol(const char* symbol, SymbolTable* currentScope)
{
    // go over all the scopes
    while (currentScope) {
        SymbolEntry* value = (SymbolEntry*)getHashMapValue(symbol, currentScope->table);
        if (value != NULL && strcmp(symbol, value->name) == 0) {
            return value;
        }
        currentScope = currentScope->parent;
    }
    return NULL;

}

SymbolTable* getClosestScope(ASTNode* root)
{
    // climb tree to get the closest scope
    SymbolTable* currentScope = NULL;
    ASTNode* node = root;
    while (node) {
        if (node->scope) {
            currentScope = node->scope;
            break;
        }
        // move up the tree
        node = node->parent;
    }
    return currentScope;
}