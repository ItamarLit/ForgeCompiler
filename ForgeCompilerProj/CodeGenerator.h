#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "AST.h"
#include "HashMap.h"

// This func will generate the asm code
void gen_asm(char* path, ASTNode* root, HashMap* stringTable, int flag);

#endif
