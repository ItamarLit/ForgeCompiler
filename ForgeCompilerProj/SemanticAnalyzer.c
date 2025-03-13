#pragma warning (disable:4996)
#include "SemanticAnalyzer.h"
#include <string.h>

/// <summary>
/// This func is used to lookup a symbol in a symbol table while traversing the AST
/// </summary>
/// <param name="symbol"></param>
/// <param name="currentScope"></param>
/// <returns>Returns the SymbolTable entry or NULL if it wasnt found</returns>
SymbolTable* lookUpSymbol(const char* symbol, SymbolTable* currentScope)
{
	while (currentScope) {
		SymbolEntry* value = (SymbolEntry*)getHashMapValue(symbol, currentScope);
		if (strcmp(symbol, value->name) == 0) {
			return value;
		}
		currentScope = currentScope->parent;
	}
	return NULL;
	
}

void resolveIdentifiers(ASTNode* root, int* errorCount) 
{

}

void checkTypes(ASTNode* root, int* errorCount) 
{

}

void checkReturn(ASTNode* root, int* errorCount) 
{

}

void checkMain(ASTNode* root, int* errorCount) 
{

}

void checkVariableInitialization(ASTNode* root, int* errorCount)
{

}

void checkFunctionCalls(ASTNode* root, int* errorCount)
{

}

void checkFunctionControlFlow(ASTNode* root, int* errorCount) 
{

}

void analyze(ASTNode* root, int* errorCount) 
{

}