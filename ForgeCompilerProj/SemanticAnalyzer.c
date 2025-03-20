#pragma warning (disable:4996)
#include "SemanticAnalyzer.h"
#include "TypeChecker.h"
#include <string.h>

/// <summary>
/// This is a helper function that goes over all children of the AST
/// </summary>
/// <param name="node"></param>
/// <param name="fn"></param>
/// <param name="errorCount"></param>
static void traverseChildren(ASTNode* node, void (*fn)(ASTNode*, int*), int* errorCount) {
    if (!node) return;
    for (int i = 0; i < node->childCount; i++) {
        fn(node->children[i], errorCount);
    }
}

/// <summary>
/// This is a helper func that will get an Expr node and return the row where the expr starts
/// </summary>
/// <param name="root"></param>
/// <returns>Returns the row where the expr starts</returns>
static int getExprLine(ASTNode* root) {
    ASTNode* node = root;
    while (!node->token) {
        node = node->children[0];
    }
    return node->token->tokenRow;
}

static void handleInvalidExpr(char* exprType, int exprLine, int* errorCount) 
{
    if (!exprType)
    {
        printf("Error: Invalid expression on line: %d\n", exprLine);
        (*errorCount)++;
    }
   
}

/// <summary>
/// This func is used to check if all the identifiers in the code have been declared before there use
/// </summary>
/// <param name="root"></param>
/// <param name="currentScope"></param>
/// <param name="errorCount"></param>
void resolveIdentifiers(ASTNode* root, int* errorCount) {
    if (!root) return;
    // climb tree to get the closest scope
    SymbolTable* currentScope = getClosestScope(root);
    // check if the type is identifier
    if (root->token && root->token->type == IDENTIFIER) {
        SymbolEntry* entry = lookUpSymbol(root->token->lexeme, currentScope);
        if (entry == NULL || strcmp(entry->type, "Error") == 0) {
            printf("Error: Undeclared identifier '%s' at line %d, column %d\n",
                root->token->lexeme, root->token->tokenRow, root->token->tokenCol);
            // insert the symbol with ERROR_TYPE so i can continue looking for problems
            if (!entry) {
                insertSymbol(currentScope->table, root->token->lexeme, "Error", 0, NULL, NULL, 0, root->token->tokenRow);
            }
            (*errorCount)++;
        }
    }
    traverseChildren(root, resolveIdentifiers, errorCount);
}

/// <summary>
/// This func is used to check that the var decleration types match the data types
/// </summary>
/// <param name="root"></param>
/// <param name="errorCount"></param>
void checkTypes(ASTNode* root, int* errorCount) 
{
    if (root == NULL) return;
    // get the closest scope
    SymbolTable* currentScope = getClosestScope(root);
    // check if the type is var dec
    if (root->lable && strcmp(root->lable, "VarDeclaration") == 0) {
        // get the var entry from the symbol table
        SymbolEntry* varEntry = lookUpSymbol(root->children[1]->token->lexeme, currentScope);
        // get the var expr type
        char* exprType = checkExprType(root->children[3]);
        // check if the expr is invalid
        handleInvalidExpr(exprType, varEntry->line, errorCount);
        // check that the types match only in defined vars
        if (exprType && varEntry != NULL && strcmp(exprType, varEntry->type) != 0) {
            printf("Error: Expr of type: %s is not valid for var of type: %s, on line: %d\n", exprType, varEntry->type, varEntry->line);
            (*errorCount)++;
        }
        // check that a var isnt init with itself ( illegal )
        if (root->children[3]->childCount == 1 && root->children[3]->children[0]->token && root->children[3]->children[0]->token->type == IDENTIFIER && varEntry && strcmp(root->children[3]->children[0]->token->lexeme, varEntry->name) == 0) {
            printf("Error: Cannot init a var: %s with itself, on line: %d\n", varEntry->name, varEntry->line);
            (*errorCount)++;
        }
    }
    traverseChildren(root, checkTypes, errorCount);
}

/// <summary>
/// This func is used to check if a node has a return statement in its children nodes
/// </summary>
/// <param name="node"></param>
/// <returns>True if a return statement was found, else false</returns>
static int containsReturn(ASTNode* node) {
    if (node == NULL) return 0;
    // check all children
    for (int i = 0; i < node->childCount; i++) {
        ASTNode* child = node->children[i];
        // found return
        if (child->lable && strcmp(child->lable, "ReturnStatement") == 0)
            return 1;
        // check child
        if (containsReturn(child)) {
            return 1;
        }
    }
    return 0; 
}

/// <summary>
/// This is a helper function that checks that all paths in a func have a return statement
/// </summary>
/// <param name="block"></param>
/// <returns>Returns true if all paths have return else false</returns>
static int functionAlwaysReturns(ASTNode* block) {
    if (block == NULL) return 0;

    int hasReturn = 0;   
    int allIfReturn = 0;
    // get the statementList block
    ASTNode* statementList = block->children[0];
    if (!statementList) return 0;
    // go through all children
    for (int i = 0; i < statementList->childCount; i++) {
        ASTNode* stmt = statementList->children[i];
        // found return
        if (stmt->lable && strcmp(stmt->lable, "ReturnStatement") == 0)
            hasReturn = 1;
        // found if statement
        if (stmt->lable && strcmp(stmt->lable, "IfStatement") == 0) {
            ASTNode* ifBlock = NULL;
            ASTNode* elseBlock = NULL;
            // get the else and the if block
            for (int j = 0; j < stmt->childCount; j++) {
                if (stmt->children[j]->lable && strcmp(stmt->children[j]->lable, "Block") == 0)
                    ifBlock = stmt->children[j];
                if (stmt->children[j]->lable && strcmp(stmt->children[j]->lable, "OptionalElse") == 0)
                    elseBlock = stmt->children[j];
            }
            // check if the if block or else block have a return
            int ifReturns = containsReturn(ifBlock);
            int elseReturns = containsReturn(elseBlock);
            // if there is an else block
            if (elseBlock) {
                // check if both blocks have return
                if (ifReturns && elseReturns) {
                    allIfReturn = 1;
                }
                // if one block is missing a return this is invalid
                else if (ifReturns && !elseReturns || elseReturns && !ifReturns){
                    // invalid missing return in one path
                    return 0;  
                }
            }
            
        }
    }
    // if all if-else have return and there is a return outside the func then it is valid
    return hasReturn || allIfReturn;
}

/// <summary>
/// This is a func that checks that a func return statement match the func return type
/// </summary>
/// <param name="node"></param>
/// <param name="retType"></param>
/// <param name="errorCount"></param>
void validateReturnExprType(ASTNode* node, const char* retType, int* errorCount, const char* funcName)
{
    if (!node) return;
    // if we found a return statement node check its return type
    if (node->lable && strcmp(node->lable, "ReturnStatement") == 0) {
        // if the func is void cant return anything
        if (strcmp(retType, "void") == 0) {
            if (node->childCount > 0) {
                printf("Error: Function: %s declared 'void' cannot return a value\n", funcName);
                (*errorCount)++;
            }
        }
        else {
            // if the func isnt void and there is no value in return then error
            if (node->childCount == 0) {
                printf("Error: Function: %s return type '%s' requires a return value\n", funcName, retType);
                (*errorCount)++;
            }
            else {
                // check the type of the returned expr
                char* returnExprType = checkExprType(node->children[0]);
                int exprRow = getExprLine(node->children[0]);
                // handle invalid expr
                handleInvalidExpr(returnExprType, exprRow, errorCount);
                if (returnExprType && strcmp(returnExprType, retType) != 0) {
                    printf("Error: Return type mismatch in function: % s. Expected '%s', got '%s' on line: %d \n",funcName, retType, returnExprType, exprRow);
                    (*errorCount)++;
                }
            }
        }
    }
    // go over all the children to find more return statemnts
    for (int i = 0; i < node->childCount; i++) {
        validateReturnExprType(node->children[i], retType, errorCount, funcName);
    }
}

/// <summary>
/// This func checks the returns in all func declerations, it checks that all paths have return and that return expr types 
/// match the func ret type
/// </summary>
/// <param name="root"></param>
/// <param name="errorCount"></param>
void checkReturn(ASTNode* root, int* errorCount) {
    if (root == NULL) return;
    // check all func dec
    for (int i = 0; i < root->childCount; i++) {
        ASTNode* node = root->children[i];
        // find func dec
        if (node->lable && strcmp(node->lable, "FuncDeclaration") == 0) {
            // get the func return type and the func name
            char* retType = node->children[2]->token->lexeme;
            char* funcName = node->children[0]->token->lexeme;
            ASTNode* body = node->children[3];
            // if type void no return needed
            if (retType && strcmp(retType, "void") == 0) continue;
            // check that func has return in all paths
            if (!functionAlwaysReturns(body)) {
                printf("Error: Function '%s' is missing a return statement in all paths\n",
                    (node->children[0]->token->lexeme));
                (*errorCount)++;
            }
            // check the functions return expr types
            validateReturnExprType(body, retType, errorCount, funcName);

        }
    }
}

/// <summary>
/// This function checks that there is a main function in the code
/// </summary>
/// <param name="root"></param>
/// <param name="errorCount"></param>
void checkMain(ASTNode* root, int* errorCount) {
    int foundMain = 0;
    // go over all children of root and check for main func
    for (int i = 0; i < root->childCount && !foundMain; i++) {
        ASTNode* node = root->children[i];
        // check if node is func dec
        if (node->lable && strcmp(node->lable, "FuncDeclaration") == 0) {
            // check the func name
            if (lookUpSymbol("Main", node->scope) != NULL) {
                foundMain = 1;
            }
        }
    }
    if (!foundMain) {
        printf("Error: No 'Main' function found\n");
        (*errorCount)++;
    }
}



/// <summary>
/// This func is used to check that the expr types of expr inside if and while blocks are bool
/// </summary>
/// <param name="root"></param>
/// <param name="errorCount"></param>
void checkBoolExprTypes(ASTNode* root, int* errorCount) 
{
    if (!root) return;
    // check if the type is bool in while statement
    if (root->lable && strcmp(root->lable, "WhileStatement") == 0) {
        char* exprType = checkExprType(root->children[0]);
        // handle invalid expr
        handleInvalidExpr(exprType, getExprLine(root->children[0]), errorCount);
        // check that the type is bool
        if (exprType && strcmp(exprType, "bool") != 0 ) {
            printf("Error: Expr type inside while must be boolean at line: %d\n", getExprLine(root));
            (*errorCount)++;
        }
    }
    // check that the type is bool in the if statements
    if (root->lable && strcmp(root->lable, "IfStatement") == 0) {
        char* exprType = checkExprType(root->children[0]);
        // handle the invalid expr
        handleInvalidExpr(exprType, getExprLine(root->children[0]), errorCount);
        if (exprType && strcmp(exprType, "bool") != 0) {
            printf("Error: Expr type inside if must be boolean at line: %d\n", getExprLine(root));
            (*errorCount)++;
        }
    }
    // go over all children
    traverseChildren(root, checkBoolExprTypes, errorCount);
}

/// <summary>
/// This function checks that the functions are called with the correct amount of variables and the correct types of vars
/// </summary>
/// <param name="root"></param>
/// <param name="errorCount"></param>
void checkFunctionCalls(ASTNode* root, int* errorCount)
{
    if (root == NULL) return;
    // climb tree to get the closest scope
    SymbolTable* currentScope = getClosestScope(root);
    // check if the type is func call
    if (root->lable && strcmp(root->lable, "FuncCallExpr") == 0) {
        ASTNode* funcCallNode = root->children[0];
        SymbolEntry* funcEntry = lookUpSymbol(funcCallNode->token->lexeme, currentScope);
        // if no entry then the func is undifined
        if (funcEntry == NULL) {
            printf("Error: Undeclared function '%s' at line %d\n",
                funcCallNode->token->lexeme, funcCallNode->token->tokenRow);
            (*errorCount)++;
        }
        else
        {
            ASTNode* argumentList = root->children[1];
            // check the param count is the same and also the param types
            if (argumentList->childCount != funcEntry->paramCount) {
                printf("Error: Missing params for function call '%s' at line %d\n",
                    funcCallNode->token->lexeme, funcCallNode->token->tokenRow);
                (*errorCount)++;
            }
            else {
                // check that the param types match
                for (int i = 0; i < funcEntry->paramCount; i++) {
                    char* argType = checkExprType(argumentList->children[i]);
                    handleInvalidExpr(argType, funcCallNode->token->tokenRow, errorCount);
                    if (argType && strcmp(argType, funcEntry->paramTypes[i]) != 0) {
                        printf("Invalid arguments for function: %s, used at line: %d, expected: '%s' but got '%s'\n", funcCallNode->token->lexeme, funcCallNode->token->tokenRow, funcEntry->paramTypes[i], argType);
                        (*errorCount)++;
                    }
                }
            }
        }
    }
    // go over all children
    traverseChildren(root, checkFunctionCalls, errorCount);
}



void analyze(ASTNode* root, int* errorCount) 
{
    // check all identifiers
    resolveIdentifiers(root, errorCount);
    // check that there is a main func
    checkMain(root, errorCount);
    // check that there is a return statement in every function and check the return types
    checkReturn(root, errorCount);
    // check that the while and if expr are bool
    checkBoolExprTypes(root, errorCount);
    // check the function calls, correct types and number of params
    checkFunctionCalls(root, errorCount);
    // check the types of the vars
    checkTypes(root, errorCount);
}