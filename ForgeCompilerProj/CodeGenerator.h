#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "AST.h"
#include "HashMap.h"

// This func will generate the asm code
void gen_asm(ASTNode* root, HashMap* stringTable);

#endif
