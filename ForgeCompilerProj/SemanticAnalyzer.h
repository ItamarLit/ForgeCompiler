#ifndef SEMANTIC_ANALYZER
#define SEMANTIC_ANALYZER

#include "AST.h"
#include "SymbolTable.h"

// this func is used to lookup a symbol from a current scope up the scopes
SymbolTable* lookUpSymbol(const char* symbol, SymbolTable* currentScope);
// this func will check that all identifiers (var names and func names) are declared before use
void resolveIdentifiers(ASTNode* root, int* errorCount);
// this func will check that all the types in the code match
void checkTypes(ASTNode* root, int* errorCount);
// this func will check functions to see that every function has a return statement
void checkReturn(ASTNode* root, int* errorCount);
// this func will check that there is a func called "Main" in the code
void checkMain(ASTNode* root, int* errorCount);
// this func will check that all vars have a value before they are used
void checkVariableInitialization(ASTNode* root, int* errorCount);
// this func will check that a function call has the correct types
void checkFunctionCalls(ASTNode* root, int* errorCount);
// this func will check that all paths have a return value in a func
void checkFunctionControlFlow(ASTNode* root, int* errorCount);

// this is the main semantic check func
void analyze(ASTNode* root, int* errorCount);

#endif
