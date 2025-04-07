#pragma warning (disable:4996)
#include <stdio.h>
#include <string.h>
#include "TypeChecker.h"
#include "Expressions.h"


// static helper functions in the checkExpr
static Type handle_identifier(ASTNode* node);
static Type handle_binary_operator(ASTNode* node);
static Type handle_unary_operator(ASTNode* node);
static Type handle_function_call(ASTNode* node);

// functions for operator handling in binary operator
static Type handle_arithmetic_op(Type leftType, Type rightType, const char* operator);
static Type handle_logical_op(Type leftType, Type rightType, const char* operator);
static Type handle_relational_op(Type leftType, Type rightType, const char* operator);
static Type handle_equality_op(Type leftType, Type rightType, const char* operator);
static Type handle_literal(ASTNode* node);

// lookup table for all binary operators
static OperatorEntry operatorTable[] = {
    {"+", handle_arithmetic_op}, 
    {"-", handle_arithmetic_op},
    {"*", handle_arithmetic_op}, 
    {"/", handle_arithmetic_op},
    {"&&", handle_logical_op}, 
    {"||", handle_logical_op},
    {"==", handle_equality_op},
    {"!=", handle_equality_op}, 
    {"<", handle_relational_op}, 
    {">", handle_relational_op},
    {"<=", handle_relational_op},
    {">=", handle_relational_op}
};

// static check functions
static int is_literal_node(ASTNode* node);
static int isIdentifierNode(ASTNode* node);
static int is_func_call_node(ASTNode* node);
static int is_unary_expression(ASTNode* node);
static int is_binary_expression(ASTNode* node);

// lookup table for the check func - handle func pairs
static ExprTypeHandler handlers[] = {
    { is_literal_node, handle_literal },
    { isIdentifierNode, (HandlerFunc)handle_identifier },
    { is_func_call_node, (HandlerFunc)handle_function_call },
    { is_unary_expression, (HandlerFunc)handle_unary_operator },
    { is_binary_expression, (HandlerFunc)handle_binary_operator },
    // mark end
    { NULL, NULL }  
};


// lookup table for the types 
static Type typeTable[] =
{
    TYPE_INT, // 0
    TYPE_STRING, // 1
    TYPE_BOOL,   // 2
};

/// <summary>
/// This is a helper function that will return true if a node is of a literal type: 
/// BOOL
/// STRING
/// INT
/// </summary>
/// <param name="node"></param>
/// <returns>Returns true if the node is a literal else false</returns>
static int is_literal_node(ASTNode* node)
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
static int is_func_call_node(ASTNode* node)
{
    return node->lable && strcmp(node->lable, "FuncCallExpr") == 0;
}

/// <summary>
/// Helper function that checks if a node is a unary expression
/// </summary>
/// <param name="node"></param>
/// <returns>Returns true if the expression is unary else false</returns>
static int is_unary_expression(ASTNode* node)
{
    return node->lable && strcmp(node->lable, "UnaryExpr") == 0;
}


/// <summary>
/// Helper function that checks if a node is a binary expression
/// </summary>
/// <param name="node"></param>
/// <returns>Returns true if the expression is binary else false</returns>
static int is_binary_expression(ASTNode* node)
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
Type check_expr_type(ASTNode* exprRoot)
{
    if (!exprRoot) return TYPE_ERROR;

    // handle expr nodes separately
    if (exprRoot->lable && strcmp(exprRoot->lable, "Expr") == 0) {
        return check_expr_type(exprRoot->children[0]);
    }
    // go over all handlers and find a match
    for (int i = 0; handlers[i].check != NULL; i++) {
        if (handlers[i].check(exprRoot)) {
            return handlers[i].handle(exprRoot);
        }
    }
    // default no match
    return TYPE_ERROR;
}
/// <summary>
/// This is a func that is used to get an identifiers type
/// </summary>
/// <param name="node"></param>
/// <param name="identifier"></param>
/// <returns>Returns the type of the identifier in string form</returns>
static Type handle_identifier(ASTNode* node)
{
    SymbolTable* currentScope = get_closest_scope(node);
    SymbolEntry* entry = lookup_symbol(node->token->lexeme, currentScope);
    if (entry) {
        return entry->type;
    }
    return TYPE_ERROR;
}

/// <summary>
/// This function handles function calls and returns the type that the func returns
/// </summary>
/// <param name="node"></param>
/// <param name="funcName"></param>
/// <returns>Returns the function return type in string form</returns>
static Type handle_function_call(ASTNode* node)
{
    SymbolTable* currentScope = get_closest_scope(node);
    SymbolEntry* entry = lookup_symbol(node->children[0]->token->lexeme, currentScope);
    if (entry) {
        return entry->returnType;
    }
    return TYPE_UNDEFINED;
}


/// <summary>
/// This is a helper func for arethmetic operators 
/// </summary>
/// <param name="leftType"></param>
/// <param name="rightType"></param>
/// <param name=""></param>
/// <returns>Returns int if the types are correct else returns error</returns>
 static Type handle_arithmetic_op(Type leftType, Type rightType, const char* operator) {
    if (leftType == TYPE_INT && rightType == TYPE_INT) {
        return TYPE_INT;
    }
    return TYPE_ERROR;
}

/// <summary>
/// This is a helper fuc for logical operators
/// </summary>
/// <param name="leftType"></param>
/// <param name="rightType"></param>
/// <param name=""></param>
/// <returns>Returns bool if the types are correct else returns error</returns>
static Type handle_logical_op(Type leftType, Type rightType, const char* operator) {
    if (leftType == TYPE_BOOL && rightType == TYPE_BOOL) {
        return TYPE_BOOL;
    }
    return TYPE_ERROR;
}

/// <summary>
/// This is a helper function for equality operators
/// </summary>
/// <param name="leftType"></param>
/// <param name="rightType"></param>
/// <param name=""></param>
/// <returns>Returns bool if types are correct else returns error</returns>
static Type handle_equality_op(Type leftType, Type rightType, const char* operator) {
    // comparasion valid for int and bool
    if ((leftType == TYPE_INT && rightType == TYPE_INT) ||
        (leftType == TYPE_BOOL && rightType == TYPE_BOOL)) {
        return TYPE_BOOL; 
    }
    return TYPE_ERROR;
}

/// <summary>
/// This is a helper function for relational operators
/// </summary>
/// <param name="leftType"></param>
/// <param name="rightType"></param>
/// <param name=""></param>
/// <returns>Returns bool if types are int else error</returns>
static Type handle_relational_op(Type leftType, Type rightType, const char* operator) {
    // both sides must be int type
    if (leftType == TYPE_INT && rightType == TYPE_INT) {
        return TYPE_BOOL; 
    }
    return TYPE_ERROR; 
}

static Type handle_binary_operator(ASTNode* node) {
    // get the left and right side types
    Type leftType = check_expr_type(node->children[0]);
    Type rightType = check_expr_type(node->children[2]);
    // get the operator type
    ASTNode* operatorNode = node->children[1];
    char* operator = operatorNode->token->lexeme;

    if (leftType == TYPE_ERROR || rightType == TYPE_ERROR) {
        return TYPE_ERROR;
    }
    // find the correct handler in the table
    for (size_t i = 0; i < (sizeof(operatorTable) / sizeof(operatorTable[0])); i++) {
        if (strcmp(operator, operatorTable[i].op) == 0) {
            return operatorTable[i].handler(leftType, rightType, operator);
        }
    }
    return TYPE_ERROR;
}


/// <summary>
/// This func is used to handler the unary expressions
/// </summary>
/// <param name="node"></param>
/// <returns>Returns the type of the unary expr or null if there is an error</returns>
static Type handle_unary_operator(ASTNode* node)
{
    // get the op node and the expr node
    ASTNode* opNode = node->children[0];
    ASTNode* exprNode = node->children[1];
    // get the type of the expr
    Type exprType = check_expr_type(exprNode);
    // check that the expr type is good for the op
    const char* opLexeme = opNode->token->lexeme;
    if (strcmp(opLexeme, "!") == 0 && exprType == TYPE_BOOL) 
    {
        return TYPE_BOOL;
    }
    else if (strcmp(opLexeme, "-") == 0 && exprType == TYPE_INT) {
        return TYPE_INT;
    }
    return TYPE_ERROR;
}

/// <summary>
/// This func handles the types of literals using the typeTable
/// </summary>
/// <param name="node"></param>
/// <returns>Returns the type of the literal</returns>
static Type handle_literal(ASTNode* node)
{
    if (!node->token) return TYPE_ERROR;
    return typeTable[node->token->type - 1];
}