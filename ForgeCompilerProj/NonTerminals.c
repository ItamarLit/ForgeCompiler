#include "NonTerminals.h"

const char* NonTerminals[] =
{
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
    "IfStatement",
    "OptionalElse",
    "WhileStatement",
    "ForStatement",
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