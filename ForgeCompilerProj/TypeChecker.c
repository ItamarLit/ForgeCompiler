#pragma warning (disable:4996)
#include <stdio.h>
#include <string.h>
#include "TypeChecker.h"
#include "Expressions.h"


// static helper functions in the checkExpr
static const char* handleIdentifier(ASTNode* node);
static const char* handleBinaryOperator(ASTNode* node);
static const char* handleUnaryOperator(ASTNode* node);
static const char* handleFunctionCall(ASTNode* node);

// functions for operator handling in binary operator
static const char* handleArithmeticOp(const char* leftType, const char* rightType, const char* operator);
static const char* handleLogicalOp(const char* leftType, const char* rightType, const char* operator);
static const char* handleComparisonOp(const char* leftType, const char* rightType, const char* operator);
static const char* handleLiteral(ASTNode* node);

// lookup table for all binary operators
static OperatorEntry operatorTable[] = {
    {"+", handleArithmeticOp}, 
    {"-", handleArithmeticOp},
    {"*", handleArithmeticOp}, 
    {"/", handleArithmeticOp},
    {"&&", handleLogicalOp}, 
    {"||", handleLogicalOp},
    {"==", handleComparisonOp}, 
    {"!=", handleComparisonOp},
    {"<", handleComparisonOp},
    {">", handleComparisonOp},
    {"<=", handleComparisonOp},
    {">=", handleComparisonOp}
};

// static check functions
static int isLiteralNode(ASTNode* node);
static int isIdentifierNode(ASTNode* node);
static int isFuncCallNode(ASTNode* node);
static int isUnaryExpression(ASTNode* node);
static int isBinaryExpression(ASTNode* node);

// lookup table for the check func - handle func pairs
static ExprTypeHandler handlers[] = {
    { isLiteralNode, handleLiteral },
    { isIdentifierNode, (HandlerFunc)handleIdentifier },
    { isFuncCallNode, (HandlerFunc)handleFunctionCall },
    { isUnaryExpression, (HandlerFunc)handleUnaryOperator },
    { isBinaryExpression, (HandlerFunc)handleBinaryOperator },
    // mark end
    { NULL, NULL }  
};

// lookup table for the types in handlerLiteral
static const char* typeTable[] =
{
    "int", // 0
    "string", // 1
    "bool",   // 2
};


/// <summary>
/// This is a helper function that will return true if a node is of a literal type: 
/// BOOL
/// STRING
/// INT
/// </summary>
/// <param name="node"></param>
/// <returns>Returns true if the node is a literal else false</returns>
static int isLiteralNode(ASTNode* node)
{
    return node->token && 
        (node->token->type == INT_LITERAL ||
         node->token->type == STRING_LITERAL ||
         node->token->type == BOOL_LITERAL);
}

/// <summary>
/// This is a helper function that checks if a node is an identifier
/// </summary>
/// <param name="node"></param>
/// <returns>Returns true if it is an identifier else false</returns>
static int isIdentifierNode(ASTNode* node)
{
    return (node->token && node->token->type == IDENTIFIER);
}

/// <summary>
/// Helper function to check if a node is a func call
/// </summary>
/// <param name="node"></param>
/// <returns>Returns true if it is a func call else false</returns>
static int isFuncCallNode(ASTNode* node)
{
    return node->lable && strcmp(node->lable, "FuncCallExpr") == 0;
}

/// <summary>
/// Helper function that checks if a node is a unary expression
/// </summary>
/// <param name="node"></param>
/// <returns>Returns true if the expression is unary else false</returns>
static int isUnaryExpression(ASTNode* node)
{
    return node->lable && strcmp(node->lable, "UnaryExpr") == 0;
}


/// <summary>
/// Helper function that checks if a node is a binary expression
/// </summary>
/// <param name="node"></param>
/// <returns>Returns true if the expression is binary else false</returns>
static int isBinaryExpression(ASTNode* node)
{
    for (int i = 0; i < ExpressionCount; i++) {
        if (node->lable && strcmp(node->lable, Expressions[i]) == 0) {
            return 1;
        }
    }
    return 0;
}




/// <summary>
/// This is the main func that is used to check an expr type
/// </summary>
/// <param name="exprRoot"></param>
/// <returns>The func will return a type in str form or NULL if an error was found</returns>
const char* checkExprType(ASTNode* exprRoot)
{
    if (!exprRoot) return NULL;

    // Handle "Expr" nodes separately
    if (exprRoot->lable && strcmp(exprRoot->lable, "Expr") == 0) {
        return checkExprType(exprRoot->children[0]);
    }

    // Iterate through the handlers and find a match
    for (int i = 0; handlers[i].check != NULL; i++) {
        if (handlers[i].check(exprRoot)) {
            return handlers[i].handle(exprRoot);
        }
    }

    return NULL; // Default case if no handler matches
}
/// <summary>
/// This is a func that is used to get an identifiers type
/// </summary>
/// <param name="node"></param>
/// <param name="identifier"></param>
/// <returns>Returns the type of the identifier in string form</returns>
static const char* handleIdentifier(ASTNode* node)
{
    SymbolTable* currentScope = getClosestScope(node);
    SymbolEntry* entry = lookUpSymbol(node->token->lexeme, currentScope);
    if (entry) {
        return entry->type;
    }
    return NULL;
}

/// <summary>
/// This function handles function calls and returns the type that the func returns
/// </summary>
/// <param name="node"></param>
/// <param name="funcName"></param>
/// <returns>Returns the function return type in string form</returns>
static const char* handleFunctionCall(ASTNode* node)
{
    SymbolTable* currentScope = getClosestScope(node);
    SymbolEntry* entry = lookUpSymbol(node->token->lexeme, currentScope);
    if (entry) {
        return entry->returnType;
    }
    return NULL;
}


/// <summary>
/// This is a helper func for arethmetic operators 
/// </summary>
/// <param name="leftType"></param>
/// <param name="rightType"></param>
/// <param name=""></param>
/// <returns>Returns int if the types are correct else returns NULL</returns>
 static const char* handleArithmeticOp(const char* leftType, const char* rightType, const char* operator) {
    if (strcmp(leftType, "int") == 0 && strcmp(rightType, "int") == 0) {
        return "int";
    }
    return NULL;
}

/// <summary>
/// This is a helper fuc for logical operators
/// </summary>
/// <param name="leftType"></param>
/// <param name="rightType"></param>
/// <param name=""></param>
/// <returns>Returns bool if the types are correct else returns NULL</returns>
static const char* handleLogicalOp(const char* leftType, const char* rightType, const char* operator) {
    if (strcmp(leftType, "bool") == 0 && strcmp(rightType, "bool") == 0) {
        return "bool";
    }
    return NULL;
}

/// <summary>
/// This is a helper func for comparision operators
/// </summary>
/// <param name="leftType"></param>
/// <param name="rightType"></param>
/// <param name=""></param>
/// <returns>Returns bool or NULL if there is an error</returns>
static const char* handleComparisonOp(const char* leftType, const char* rightType, const char* operator) {
    // dont allow string comparisions
    if (strcmp(leftType, "string") == 0 || strcmp(rightType, "string") == 0) {
        return NULL;
    }
    // if both types are the same the result is a bool
    if (strcmp(leftType, rightType) == 0) {
        return "bool";
    }
    return NULL;
}

static const char* handleBinaryOperator(ASTNode* node) {
    // get the left and right side types
    const char* leftType = checkExprType(node->children[0]);
    const char* rightType = checkExprType(node->children[2]);
    // get the operator type
    ASTNode* operatorNode = node->children[1];
    char* operator = operatorNode->token->lexeme;

    if (leftType == NULL || rightType == NULL) {
        return NULL;
    }
    // find the correct handler in the table
    for (size_t i = 0; i < (sizeof(operatorTable) / sizeof(operatorTable[0])); i++) {
        if (strcmp(operator, operatorTable[i].op) == 0) {
            return operatorTable[i].handler(leftType, rightType, operator);
        }
    }
    // if the operator wasnt found then it is an invalid operator
    return NULL;
}


/// <summary>
/// This func is used to handler the unary expressions
/// </summary>
/// <param name="node"></param>
/// <returns>Returns the type of the unary expr or null if there is an error</returns>
static const char* handleUnaryOperator(ASTNode* node)
{
    // get the op node and the expr node
    ASTNode* opNode = node->children[0];
    ASTNode* exprNode = node->children[1];
    // get the type of the expr
    char* exprType = checkExprType(exprNode);
    // check that the expr type is good for the op
    const char* opLexeme = opNode->token->lexeme;
    if (strcmp(opLexeme, "!") == 0 && strcmp(exprType, "bool") == 0) 
    {
        return "bool";
    }
    else if (strcmp(opLexeme, "-") == 0 && strcmp(exprType, "int") == 0) {
        return "int";
    }
    return NULL;
}

/// <summary>
/// This func handles the types of literals using the typeTable
/// </summary>
/// <param name="node"></param>
/// <returns>Returns the type of the literal</returns>
static const char* handleLiteral(ASTNode* node)
{
    if (!node->token) return NULL;
    return typeTable[node->token->type - 1];
}