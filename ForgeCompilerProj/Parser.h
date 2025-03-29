#ifndef PARSER_H 
#define PARSER_H

#include "HashMap.h"
#include "Token.h"
#include "AST.h"

// struct for the hashmap key for, goto and action tables
typedef struct MapKey {
    int currentState;
    char* symbol;
} MapKey;


// func for init of action table from file
void InitActionTable(HashMap** map, char* filename);
// func for init of goto table from file
void InitGotoTable(HashMap** map, char* filename);
// main parse function
ASTNode* ParseInput(pTokenArray tokenArray, int* errorCount);

#endif