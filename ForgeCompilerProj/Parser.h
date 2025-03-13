#ifndef PARSER_H 
#define PARSER_H

#include "HashMap.h"
#include "Token.h"
#include "AST.h"

#define STATE_BEFORE_EMPTY_PARAMLIST  60   // Before shifting empty for ( )
#define STATE_AFTER_EMPTY_PARAMLIST   92   // After shifting empty for ( ) in FuncDecl

#define STATE_BEFORE_EMPTY_ARGLIST    58       // Before shifting empty for ( ) in FuncCall
#define STATE_AFTER_EMPTY_ARGLIST     88  // After shifting empty for ( ) in FuncCall

#define STATE_BEFORE_EMPTY_BLOCK      136  // Before shifting empty for { }
#define STATE_AFTER_EMPTY_BLOCK       144  // After shifting empty for { } in a function or statement block

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