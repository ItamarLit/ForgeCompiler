#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "HashMap.h"
#include "Types.h"
#define IGNORE_LINE -1

struct ASTNode; //forward declare the ASTNode

typedef enum {
    IS_GLOBAL,
    IS_LOCAL,
    IS_REG,
} Placement;


// value for the hashmap that will represent the table
typedef struct SymbolEntry {
    char* name;           // Variable or function name
    Type type;           // Data type 
    int line;              // line num, used for better errors
    int isFunction;      // 1 if it's a function, 0 otherwise
    Type returnType;    // Function return type
    Type* paramTypes;  // array of param types for functions
    int paramCount;     // param count for functions
    Placement place;    // enum for var placements
    int offset;         // int value for offset, 0 - RCX, 1 - RDX, 2 - R8, 3 - R9, affter that offset in bytes
} SymbolEntry;

typedef struct SymbolTable {
    HashMap* table;              // symbol table
    struct SymbolTable* parent;  // parent scope (NULL if global)
    int localOffset;            // counter for local vars offset
} SymbolTable;

// func that will traverse AST and build the symbol tables
void create_AST_symbol_table(struct ASTNode* node, SymbolTable* currentTable, int* errorCount);
// func that creates new symbol tables
SymbolTable* create_new_scope(SymbolTable* parent);
// func for printing the symbol tables
void print_symbol_tables(struct ASTNode* node);
// this func is used to get the closest scope
SymbolTable* get_closest_scope(struct ASTNode* node);
// this func is used to lookup a symbol from a current scope up the scopes
SymbolEntry* lookup_symbol(const char* symbol, SymbolTable* currentScope);
// this func is used to insert a symbol into the symbol table
void insert_symbol(HashMap* map, char* name, Type type, int isFunction, Type returnType, Type* paramTypes, int paramCount, int line, Placement place, int offset);


#endif