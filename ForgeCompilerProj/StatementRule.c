#include "StatementRule.h"
#pragma warning (disable:4996)
#include <stdlib.h>
#include <string.h>

StatementRule statementTable[] = {
    { "mold",   17 },   
    { "forge",  22 },
    { "remold", 29 },
    { "return", 36 },
    { "if",     37 },
    { "else",   39 },
    { "while",  40 },
    { "for",    41 },
    { "input",  44 },
    { "output", 45 },
    // finishing sign
    { NULL,     -1 }
};

int GetStatementRuleIndex(const char* token)
{
    // search the array
    for (int i = 0; statementTable[i].token != NULL; i++) {
        if (strcmp(token, statementTable[i].token) == 0) {
            return statementTable[i].ruleIndex;
        }
    }
    // not found
    return -1; 
}