#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "AST.h"
#include "SymbolTable.h"
#include "Types.h"


// operator handler
typedef Type (*OperatorHandler)(Type, Type, const char*);
// regular handler in the check expr func
typedef Type (*HandlerFunc)(ASTNode*);
// check func that is used in the check expr func
typedef int (*CheckFunc)(ASTNode*);

// struct for operator and handler func, used to minimize if else usage
typedef struct {
    char* op;
    OperatorHandler handler;
} OperatorEntry;

// struct for the checkfunc - handler pairs
typedef struct {
    CheckFunc check;
    HandlerFunc handle;
} ExprTypeHandler;


// this func is used to check the type of any given expression
Type checkExprType(ASTNode* exprRoot);


#endif 
