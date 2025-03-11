#ifndef AST_H
#define AST_H
#include "Token.h"

typedef struct ASTNode {
    char* lable; // name for non terminals
    Token* token; // node token
    struct ASTNode** children; // dynamic array of ast nodes 
    int childCount; // size of the array
} ASTNode;

// func that creates the ast node 
ASTNode* createASTNode(Token* token, const char* lable);
// func that frees an ast node and all of its children
void freeASTNode(ASTNode* node);
// func that fills an ast nodes children 
void addChild(ASTNode* child, ASTNode* parent);

#endif
