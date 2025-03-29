#ifndef SEMANTIC_ANALYZER
#define SEMANTIC_ANALYZER

#include "AST.h"
#include "SymbolTable.h"

// this is the main semantic check func
void analyze(ASTNode* root, int* errorCount);

#endif
