#include "NonTerminals.h"

const char* NonTerminals[] = 
{
    "Program'",
    "Program",
    "DeclarationList",
    "StatementList",
    "Declaration",
    "VarDeclaration",
    "Type",
    "Statement",
    "AssignmentStatement",
    "AssignOp",
    "ReturnStatement",
    "OptionalExpr",
    "IfStatement",
    "OptionalElse",
    "WhileStatement",
    "ForStatement",
    "IOStatement",
    "InputStatement",
    "OutputStatement",
    "Block", 
    "FuncDeclaration",
    "ReturnType",
    "ParamList",
    "ParamDecl", 
    "Expr", 
    "OrExpr", 
    "AndExpr",
    "EqualityExpr",
    "RelationalExpr",
    "AddExpr", 
    "MulExpr", 
    "UnaryExpr",
    "FuncCallExpr", 
    "ArgumentList", 
    "PrimaryExpr"
};

// get the non terminal count
const int NonTerminalCount = sizeof(NonTerminals) / sizeof(NonTerminals[0]);