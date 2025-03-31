#ifndef STRING_TABLE_H
#define STRING_TABLE_H

#include "AST.h"
#include "HashMap.h"

// Struct for string entry in table
typedef struct  {
    char* label;
    int originalLen;
} StringEntry;

// This func will be used to create the string table itself
HashMap* createStringTable(ASTNode* root);
// This func will be used to get a label that represents a string or NULL if the string isnt in the table
char* lookUpString(char* string, HashMap* map);

#endif