#ifndef CODE_GEN_UTILS_H
#define CODE_GEN_UTILS_H

#include "SymbolTable.h"
#include "HashMap.h"
#include "AST.h"

// handler function
typedef void (*GenHandler)(ASTNode* node, HashMap* stringTable);
// struct for handler table
typedef struct {
    const char* label;
    GenHandler handler;
} HandlerEntry;

// scratch reg count
#define REG_COUNT 7
// This func will return a number that represents a free register
int scratch_alloc();
// This func will free a register that was inuse
void scratch_free(int reg);
// This func will return a reg name based on the reg number
const char* scratch_name(int reg);
// This func will format new label names
const char* label_name();
// This func will create the code gen for symbols
const char* symbol_codegen(SymbolEntry* s);
// This func will create a new ASM file
void createAsmFile();
// This func will add a new line to the asm file
void insert_line(const char* fmt, ...);

#endif
