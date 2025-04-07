#pragma warning (disable:4996)
#include "SymbolTable.h"
#include "AST.h"
#include "ErrorHandler.h"
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
    unsigned long hash = djb2Hash(fkey);
    return hash % map_size;
}

/// <summary>
/// This function is used to insert a new entry into a given map
/// </summary>
/// <param name="map"></param>
/// <param name="name"></param>
/// <param name="type"></param>
/// <param name="isFunction"></param>
/// <param name="returnType"></param>
/// <param name="paramTypes"></param>
/// <param name="paramCount"></param>
/// <param name="line"></param>
void insertSymbol(HashMap* map, char* name, Type type, int isFunction, Type returnType, Type* paramTypes, int paramCount, int line, Placement place, int offset) {
    // Allocate and fill SymbolEntry struct
    SymbolEntry* entry = malloc(sizeof(SymbolEntry));
    if (!entry) {
        fprintf(stderr, "Failed to malloc entry for symbol table\n");
        return;
    }
    entry->name = strdup(name);
    entry->type = type;
    entry->line = line;
    entry->isFunction = isFunction;
    entry->returnType = returnType;
    if (paramCount > 0 && paramTypes != NULL) {
        entry->paramTypes = (Type*)malloc(sizeof(Type) * paramCount);
        if (!entry->paramTypes)
        {
            fprintf(stderr, "Failed to malloc entry param types arr\n");
            free(name);
            free(entry);
            return;
        }
        for (int i = 0; i < paramCount; i++) {
            entry->paramTypes[i] = paramTypes[i];
        }
    }
    else {
        entry->paramTypes = NULL;
    }
    entry->paramCount = paramCount;
    entry->place = place;
    entry->offset = offset;
    // insert value in hashmap
    insertNewValue(strdup(name), entry, map);
}

/// <summary>
/// This func returns a map value
/// </summary>
/// <param name="map"></param>
/// <param name="name"></param>
/// <returns>Returns the map value based on a name</returns>
static SymbolEntry* getMapValue(HashMap* map, char* name) {
    return (SymbolEntry*)getHashMapValue(name, map);
}

void freeSymbolEntry(void* value) {
    SymbolEntry* entry = (SymbolEntry*)value;
    free(entry->name);
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
        entry->name, convertTypeToString(entry->type), entry->isFunction, entry->paramCount);
    if (entry->paramCount == 0) {
        printf("No params ]");
        return;
    }
    for (int i = 0; i < entry->paramCount - 1; i++) {
        printf("%s, ", convertTypeToString(entry->paramTypes[i]));
    }
    printf("%s ]", convertTypeToString(entry->paramTypes[entry->paramCount - 1]));
}

/// <summary>
/// This func is used to get function parameters
/// </summary>
/// <param name="paramNode"></param>
/// <param name="paramNames"></param>
/// <param name="paramTypes"></param>
/// <returns>Returns a count of the amount of parameters</returns>
int extractFunctionParameters(ASTNode* paramNode, char*** paramNames, Type** paramTypes) {
    // get children count
    int childrenCount = paramNode->childCount;
    if (childrenCount == 0) {
        return 0;
    }
    int decCount = 0;
    // go over all children of the param list
    for (int i = 0; i < childrenCount; i++) {
        ASTNode* paramDecl = paramNode->children[i];
        // get the type and identifier from the ParamDecl nodes
        if (strcmp(paramDecl->lable, "ParamDecl") == 0) {
            // realloc the arrays
            char** temp = (char**)realloc(*paramNames, sizeof(char*) * (decCount + 1));
            if (!temp) 
            {
                fprintf(stderr, "Error reallocing param names array in extraction func\n");
                return NULL;
            }
            *paramNames = temp;
            Type* tempType = (Type*)realloc(*paramTypes, sizeof(Type) * (decCount + 1));
            if (!tempType) 
            {
                fprintf(stderr, "Error reallocing param types array in extraction func\n");
                return NULL;
            }
            *paramTypes = tempType;
            ASTNode* typeNode = paramDecl->children[0];
            (*paramTypes)[decCount] = convertStringType(typeNode->token->lexeme);
            ASTNode* nameNode = paramDecl->children[1];
            (*paramNames)[decCount] = strdup(nameNode->token->lexeme);
            decCount++;
        }
    }
    return decCount;
}


/// <summary>
/// This func creates a new scope (symbol table) and sets its parent
/// </summary>
/// <param name="parent"></param>
/// <returns>Returns a pointer to the newly created table</returns>
SymbolTable* createNewScope(SymbolTable* parent) {
    SymbolTable* newTable = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!newTable) {
        fprintf(stderr, "Unable to malloc memory for symbol table\n");
        return NULL;
    }
    newTable->table = initHashMap(INITAL_HASHMAP_SIZE, hashFunc, equalFunc, printStringKey, printSymbolEntry, free, freeSymbolEntry);
    newTable->parent = parent;  // link the current table to its parent
    newTable->localOffset = 0;
    return newTable;
}


char* change_name(const char* name) {
    char* new_name = (char*)malloc(strlen(name) + 2);
    if (!new_name) 
    {
        fprintf(stderr, "Unable to malloc memory for name change\n");
        return NULL;
    }
    sprintf(new_name, "_%s", name);
    return new_name;
}

/// <summary>
/// This is the main func that creates the symbol table "Tree"
/// </summary>
/// <param name="node"></param>
/// <param name="currentTable"></param>
/// <param name="errorCount"></param>
void createASTSymbolTable(ASTNode* node, SymbolTable* currentTable, int* errorCount) {
    if (!node) return;

    if (node->lable && strcmp(node->lable, "VarDeclaration") == 0) {
        // get the var name
        char* varName = node->children[1]->token->lexeme;  
        char* newVarName = change_name(varName);
        // get the var type
        char* varType = node->children[0]->token->lexeme;  
        // check if the var exists already in the table
        if (getMapValue(currentTable->table, newVarName)) {
            output_error(SEMANTIC, "Variable % s already declared in this scope\n", varName);
            (*errorCount)++;
        }
        else {
            Placement place;
            int offset = 0;
            // check if the var Dec is global or local using parent
            if (currentTable->parent == NULL) {
                // global
                place = IS_GLOBAL;
            }
            else {
                // local
                place = IS_LOCAL;
                if (strcmp(varType, "string") == 0) {
                    // reserve 72 bytes on stack
                    currentTable->localOffset -= 72;
                }
                else 
                {
                    // reserve 8 bytes on stack
                    currentTable->localOffset -= 8;
                }
                offset = currentTable->localOffset;
            }
            // insert the symbol
            insertSymbol(currentTable->table, newVarName, convertStringType(varType), 0, TYPE_UNDEFINED, NULL, 0, node->children[1]->token->tokenRow, place, offset);
        }
        free(newVarName);

    }
    // handle new scope for functions
    else if ( node->lable && strcmp(node->lable, "FuncDeclaration") == 0) {
        // get the function name
        char* funcName = node->children[0]->token->lexeme; 
        char* newFuncName = change_name(funcName);
        // get the paramlist node
        ASTNode* paramList = node->children[1]; 
        // get the return type
        char* returnType = node->children[2]->token->lexeme; 
        // check if the func is declared
        if (getMapValue(currentTable->table, newFuncName)) {
            output_error(SEMANTIC, "Function %s already declared\n", newFuncName);
            (*errorCount)++;
        }
        // go over all param dec in func 
        char** paramNames = NULL;
        Type* paramTypes = NULL;
        // get the types and names
        int decCount = extractFunctionParameters(paramList, &paramNames, &paramTypes);
        // insert the func into the table
        insertSymbol(currentTable->table, newFuncName, convertStringType(returnType), 1, convertStringType(returnType), paramTypes, decCount, IGNORE_LINE, IS_GLOBAL, -1);
        free(newFuncName);
        // create new symbol table for func scope
        SymbolTable* functionScope = createNewScope(currentTable);
        node->scope = functionScope;      
        // fill the function scope with the params
        for (int i = 0; i < decCount; i++) {
            // check what size is needed for param
            int sizeNeeded = (paramTypes[i] == TYPE_STRING) ? 72 : 8;
            functionScope->localOffset -= sizeNeeded;
            // treat all params as local
            int offset = functionScope->localOffset;
            Placement place = IS_LOCAL; 
            // insert params
            char* newVarName = change_name(paramNames[i]);
            insertSymbol(functionScope->table, newVarName, paramTypes[i], 0,TYPE_UNDEFINED, NULL, 0,IGNORE_LINE, place, offset);
            free(paramNames[i]);
            free(newVarName);
        }
        if (paramNames) {
            free(paramNames);
        }
        if (paramTypes) {
            free(paramTypes);
        }
        // go over function body
        createASTSymbolTable(node->children[3], functionScope, errorCount);
    }
    // handle new scope for blocks
    else if (node->lable && strcmp(node->lable, "Block") == 0) {
        // create a new symbol table for this block
        SymbolTable* blockScope = createNewScope(currentTable);
        node->scope = blockScope;
        // go over statements in the block
        for (int i = 0; i < node->childCount; i++) {
            createASTSymbolTable(node->children[i], blockScope, errorCount);
        }
    }
    else {
        // go over the children
        for (int i = 0; i < node->childCount; i++) {
            createASTSymbolTable(node->children[i], currentTable, errorCount);
        }
    }
}

/// <summary>
/// This is a helper function used to debug and see the tables
/// </summary>
/// <param name="node"></param>
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
    char* newName = change_name(symbol);
    // go over all the scopes
    while (currentScope && newName) {
        SymbolEntry* value = (SymbolEntry*)getHashMapValue(newName, currentScope->table);
        if (value != NULL && strcmp(newName, value->name) == 0) {
            return value;
        }
        currentScope = currentScope->parent;
    }
    free(newName);
    return NULL;

}

/// <summary>
/// This func is used to get the closest scope to a node
/// </summary>
/// <param name="root"></param>
/// <returns>Returns the scope if it found one or NULL if it didnt</returns>
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


