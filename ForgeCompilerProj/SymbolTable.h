#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "HashMap.h"

struct ASTNode; //forward declare the ASTNode


// value for the hashmap that will represent the table
typedef struct SymbolEntry {
    char* name;           // Variable or function name
    char* type;           // Data type 
    int isFunction;      // 1 if it's a function, 0 otherwise
    char* returnType;    // Function return type
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

#endif