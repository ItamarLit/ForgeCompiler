#ifndef AST_H
#define AST_H
#include "Token.h"

typedef struct ASTNode {
    Token* token; // node token
    struct ASTNode** children; // dynamic array of ast nodes 
    int childCount; // size of the array
} ASTNode;

// func that creates the ast node 
ASTNode* createASTNode(ASTNode** children, int childCount);
// func that frees an ast node and all of its children
void freeASTNode(ASTNode* node);
#endif
