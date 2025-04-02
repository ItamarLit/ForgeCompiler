#ifndef EXPR_CODE_GEN_H
#define EXPR_CODE_GEN_H

#include "AST.h"
#include "HashMap.h"

// struct for arethmitic
typedef struct {
    const char* label;
    const char* op;
} BinEntry;

// this func will generate expressions code
void gen_expr(ASTNode* node, HashMap* stringTable);

#endif
