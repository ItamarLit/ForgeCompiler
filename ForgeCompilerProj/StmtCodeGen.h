#ifndef STMT_CODE_GEN_H
#define STMT_CODE_GEN_H

#include "AST.h"
#include "HashMap.h"

// handler for assignment statements
typedef void (*AssignHandler)(SymbolEntry* entry, int r1);
// struct for table
typedef struct {
    const char* op;
    AssignHandler handler;
} AssignEntry;

// this func will generate statement code
void gen_statment(ASTNode* node, HashMap* stringTable);
// this func will generate statement lists code
void gen_statement_list(ASTNode* node, HashMap* stringTable);

#endif
