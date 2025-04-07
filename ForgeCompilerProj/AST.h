#ifndef AST_H
#define AST_H

#include "Token.h"
#include "SymbolTable.h"

typedef struct ASTNode {
    char* lable; // name for non terminals
    Token* token; // node token
    struct ASTNode** children; // dynamic array of ast nodes 
    int childCount; // size of the array
    SymbolTable* scope; // symbol table 
    struct ASTNode* parent; // parent pointer
    int reg;                // register used in code gen      
} ASTNode;

// func that creates the ast node 
ASTNode* create_AST_node(Token* token, const char* lable);
// func that frees an ast node and all of its children
void free_AST_node(ASTNode* node);
// func that fills an ast nodes children 
void add_child(ASTNode* child, ASTNode* parent);
// func that prints the ast 
void print_AST(ASTNode* root, int tabCount);
// func that compresses the AST
ASTNode* compress_AST(ASTNode* node);
// func that normalizes AST
void normalize_AST(ASTNode* node);
// func that reduces global vars
void reduce_global_vars(ASTNode* globalItemList);

#endif
