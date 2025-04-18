#include "NonTerminals.h"

// array of non terminals 
const char* NonTerminals[] = {
    "Program'",
    "Program",
    "GlobalItemList",
    "GlobalItem",
    "Statement",
    "StatementList",
    "VarDeclaration",
    "Type",
    "FuncDeclaration",
    "ReturnType",
    "ParamList",
    "ParamDecl",
    "AssignmentStatement",
    "AssignOp",
    "ReturnStatement",
    "IfElseStatement",
    "IfStatement",
    "WhileStatement",
    "IOStatement",
    "InputStatement",
    "OutputStatement",
    "Block",
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