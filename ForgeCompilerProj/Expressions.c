#include "Expressions.h"


// array of expressions
const char* Expressions[] = {
    "OrExpr",
    "AndExpr",
    "EqualityExpr",
    "RelationalExpr",
    "AddExpr",
    "MulExpr",
};


const int ExpressionCount = sizeof(Expressions) / sizeof(Expressions[0]);
