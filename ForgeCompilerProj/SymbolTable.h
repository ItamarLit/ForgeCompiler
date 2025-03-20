#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "HashMap.h"
#include "Types.h"
#define IGNORE_LINE -1

struct ASTNode; //forward declare the ASTNode


// value for the hashmap that will represent the table
typedef struct SymbolEntry {
    char* name;           // Variable or function name
    Type type;           // Data type 
    int line;              // line num, used for better errors
    int isFunction;      // 1 if it's a function, 0 otherwise
    Type returnType;    // Function return type
    Type* paramTypes;  // array of param types for functions
    int paramCount;     // param count for functions
} SymbolEntry;

typedef struct SymbolTable {
    HashMap* table;              // symbol table
    struct SymbolTable* parent;  // parent scope (NULL if global)
} SymbolTable;

// func that will traverse AST and build the symbol tables
void createASTSymbolTable(struct ASTNode* node, SymbolTable* currentTable, int* errorCount);
// func that creates new symbol tables
SymbolTable* createNewScope(SymbolTable* parent);
// func for printing the symbol tables
void printSymbolTables(struct ASTNode* node);
// this func is used to get the closest scope
SymbolTable* getClosestScope(struct ASTNode* node);
// this func is used to lookup a symbol from a current scope up the scopes
SymbolEntry* lookUpSymbol(const char* symbol, SymbolTable* currentScope);
// this func is used to insert a symbol into the symbol table
void insertSymbol(HashMap* map, char* name, Type type, int isFunction, Type returnType, Type* paramTypes, int paramCount, int line);

#endif