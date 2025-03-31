#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "AST.h"
#include "HashMap.h"

// this func will generate the data section of the final asm file
void gen_data_seg(ASTNode* root);
// this func will generate the read only data section 
void gen_readOnly_data_seg(HashMap* stringTable);
// this func will generate the stack section of the final asm file
void gen_stack_seg();

#endif
