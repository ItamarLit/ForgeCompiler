#ifndef PARSER_H 
#define PARSER_H

#include "HashMap.h"
#include "Token.h"
#include "AST.h"

#define STATE_BEFORE_EMPTY_PARAMLIST  64   // Before shifting empty for ( )
#define STATE_AFTER_EMPTY_PARAMLIST   95   // After shifting empty for ( ) in FuncDecl

#define STATE_BEFORE_EMPTY_ARGLIST    61       // Before shifting empty for ( ) in FuncCall
#define STATE_AFTER_EMPTY_ARGLIST     90  // After shifting empty for ( ) in FuncCall

#define STATE_BEFORE_EMPTY_BLOCK      137  // Before shifting empty for { }
#define STATE_AFTER_EMPTY_BLOCK       146  // After shifting empty for { } in a function or statement block

#define STATE_BEFORE_EMPTY_ELSE      136  // Before shifting empty for { }
#define STATE_AFTER_EMPTY_ELSE      142  // After shifting empty for { } in a function or statement block

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