#ifndef SEMANTIC_ANALYZER
#define SEMANTIC_ANALYZER

#include "AST.h"
#include "SymbolTable.h"


// this func will check that all identifiers (var names and func names) are declared before use
void resolveIdentifiers(ASTNode* root, int* errorCount);
// this func will check that all the types in the code match
void checkTypes(ASTNode* root, int* errorCount);
// this func will check functions to see that every function has a return statement
void checkReturn(ASTNode* root, int* errorCount);
// this func will check that there is a func called "Main" in the code
void checkMain(ASTNode* root, int* errorCount);
// this func will check that a function call has the correct types
void checkFunctionCalls(ASTNode* root, int* errorCount);
// this func will check that the expr in if and while are bool
void checkBoolExprTypes(ASTNode* root, int* errorCount);
// this is the main semantic check func
void analyze(ASTNode* root, int* errorCount);

#endif
