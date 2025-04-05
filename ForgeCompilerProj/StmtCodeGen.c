#pragma warning (disable:4996)
#include "StmtCodeGen.h"
#include "ExprCodeGen.h"
#include "CodeGenUtils.h"
#include "SymbolTable.h"
#include "TypeChecker.h"
#include <stdio.h>

void gen_statement_list(ASTNode* node, HashMap* stringTable);
static void gen_var_dec(ASTNode* node, HashMap* stringTable);
static void gen_assignment_dec(ASTNode* node, HashMap* stringTable);
static void gen_if_statement(ASTNode* node, HashMap* stringTable);
static void gen_while_statement(ASTNode* node, HashMap* stringTable);
static void gen_return_statement(ASTNode* node, HashMap* stringTable);
static void gen_func_call(ASTNode* node, HashMap* stringTable);
static void gen_output_call(ASTNode* node, HashMap* stringTable);
static void gen_input_call(ASTNode* node, HashMap* stringTable);

void handle_assign(SymbolEntry* entry, int r1);
void handle_add_assign(SymbolEntry* entry, int r1);
void handle_add_assign(SymbolEntry* entry, int r1);
void handle_sub_assign(SymbolEntry* entry, int r1);
void handle_mul_assign(SymbolEntry* entry, int r1);
void handle_div_assign(SymbolEntry* entry, int r1);

// This table matches statement to handler func
static HandlerEntry stmtTable[] =
{
    {"VarDeclaration", gen_var_dec},
    {"AssignmentStatement", gen_assignment_dec},
    {"IfStatement", gen_if_statement},
    {"IfElseStatement", gen_if_statement},
    {"WhileStatement", gen_while_statement},
    {"ReturnStatement", gen_return_statement},
    {"FuncCallExpr", gen_func_call},
    {"OutputStatement", gen_output_call},
    {"InputStatement", gen_input_call},
    {NULL, NULL}
};

// This table matches assign op to handler func
static AssignEntry assignTable[] = {
    {"=", handle_assign},
    {"+=", handle_add_assign},
    {"-=", handle_sub_assign},
    {"*=", handle_mul_assign},
    {"/=", handle_div_assign},
    {NULL, NULL} 
};

/// <summary>
/// This is the main function that creates statment code
/// </summary>
/// <param name="node"></param>
/// <param name="stringTable"></param>
void gen_statment(ASTNode* node, HashMap* stringTable)
{
    if (!node) return;
    // go over all labels and check what func to run
    for (int i = 0; stmtTable[i].label != NULL; i++) {
        if (strcmp(node->lable, stmtTable[i].label) == 0) {
            stmtTable[i].handler(node, stringTable);
            return;
        }
    }
}

/// <summary>
/// This func generates code for a local var dec
/// </summary>
/// <param name="node"></param>
/// <param name="stringTable"></param>
static void gen_var_dec(ASTNode* node, HashMap* stringTable)
{
    SymbolTable* currentScope = getClosestScope(node);
    SymbolEntry* entry = lookUpSymbol(node->children[1]->token->lexeme, currentScope);
    if (entry->place != IS_LOCAL) return;

    // generate the expression (r1 will hold the result)
    gen_expr(node->children[3], stringTable);
    int r1 = node->children[3]->reg;
    // store the value in the stack (pointer for strings)
    insert_line("mov [rbp + %d], %s\n", entry->offset, scratch_name(r1));
    scratch_free(r1);
}

/// <summary>
/// This func generates code for an assignment
/// </summary>
/// <param name="node"></param>
/// <param name="stringTable"></param>
static void gen_assignment_dec(ASTNode* node, HashMap* stringTable)
{
    gen_expr(node->children[2], stringTable);
    int r1 = node->children[2]->reg;

    SymbolTable* currentScope = getClosestScope(node);
    SymbolEntry* entry = lookUpSymbol(node->children[0]->token->lexeme, currentScope);
    // "+=", "-=", "*=", "/=", "="
    const char* assignOp = node->children[1]->lable; 
    // go over the table and match the handler
    for (int i = 0; assignTable[i].op != NULL; i++) {
        if (strcmp(assignOp, assignTable[i].op) == 0) {
            assignTable[i].handler(entry, r1);
            break;
        }
    }
    scratch_free(r1);
}

/// <summary>
/// This func generates if statement code
/// </summary>
/// <param name="node"></param>
/// <param name="stringTable"></param>
void gen_if_statement(ASTNode* node, HashMap* stringTable) {
    // get labels
    char* falseLabel = label_name();
    char* doneLabel = label_name();
    // gen the condition
    gen_expr(node->children[0], stringTable);
    // cmp the result with false
    insert_line("cmp %s, 0\n", scratch_name(node->children[0]->reg));
    insert_line("je %s\n", falseLabel);
    // free condition reg
    scratch_free(node->children[0]->reg);
    // if true gen the statements
    gen_statement_list(node->children[1]->children[0], stringTable);
    // jmp to done
    insert_line("jmp %s\n", doneLabel);
    // false label
    insert_line("%s:\n", falseLabel);
    free(falseLabel);
    // handle else block if it exists
    if (node->childCount == 3) {
        // gen else statements
        gen_statement_list(node->children[2], stringTable);
    }

    // done label
    insert_line("%s:\n", doneLabel);
    free(doneLabel);
}

/// <summary>
/// This func generates while stataement code
/// </summary>
/// <param name="node"></param>
/// <param name="stringTable"></param>
void gen_while_statement(ASTNode* node, HashMap* stringTable) {
    // gen two labels
    char* startLabel = label_name();
    char* doneLabel = label_name();
    // loop start
    insert_line("%s:\n", startLabel);
    // gen condition
    gen_expr(node->children[0], stringTable);
    // check if condition is false
    insert_line("cmp %s, 0\n", scratch_name(node->children[0]->reg));
    // exit if false
    insert_line("je %s\n", doneLabel);
    // free condition reg
    scratch_free(node->children[0]->reg);
    // gen body
    gen_statement_list(node->children[1]->children[0], stringTable);
    // loop back
    insert_line("jmp %s\n", startLabel);
    // done label
    insert_line("%s:\n", doneLabel);
    free(doneLabel);
    free(startLabel);
}

/// <summary>
/// This is a helper function that returns the func node (the node that holds all the func dec children)
/// </summary>
/// <param name="node"></param>
/// <returns></returns>
ASTNode* getEnclosingFunction(ASTNode* node) {
    while (node) {
        if (strcmp(node->lable, "FuncDeclaration") == 0)
            return node;
        node = node->parent;
    }
    // should never happen
    return NULL;
}

/// <summary>
/// This func generates a return statement in a function
/// </summary>
/// <param name="node"></param>
/// <param name="stringTable"></param>
void gen_return_statement(ASTNode* node, HashMap* stringTable)
{
    // get the main func node
    ASTNode* funcNode = getEnclosingFunction(node);
    // get the func name
    const char* funcName = funcNode->children[0]->token->lexeme;
    // no return data ( void func )
    if (node->childCount == 0) {
        printf("jmp _%s_ret\n", funcName);
        return;
    }
    gen_expr(node->children[1], stringTable);
    // get the reg 
    int r = node->children[1]->reg;
    // move return data to rax
    insert_line("mov rax, %s\n", scratch_name(r));  
    // if the return is a local string then copy to global buffer
    if (node->children[1]->token->type == IDENTIFIER) 
    {
        SymbolTable* scope = getClosestScope(node);
        SymbolEntry* entry = lookUpSymbol(node->children[1]->token->lexeme, scope);
        if (entry->type == TYPE_STRING && entry->place == IS_LOCAL) 
        {
            // copy the string into the global buffer
            insert_line("mov rsi, %s", scratch_name(r));
            insert_line("mov rdi, offset global_copy_buffer"); 
            insert_line("call copy_string");
            insert_line("mov rax, offset global_copy_buffer");
        }
    }
    // free the reg
    scratch_free(r);
    // go to the return
    insert_line("jmp _%s_ret\n", funcName);
    return;
}

/// <summary>
/// This func generates code for a func call
/// </summary>
/// <param name="node"></param>
/// <param name="stringTable"></param>
static void gen_func_call(ASTNode* node, HashMap* stringTable) 
{
    // calls are expressions, but can be used as statements
    gen_expr(node, stringTable);
    scratch_free(node->reg);
}

/// <summary>
/// This func generates code for a statement list 
/// </summary>
/// <param name="node"></param>
/// <param name="stringTable"></param>
void gen_statement_list(ASTNode* node, HashMap* stringTable) {
    if (!node) return;
    if (strcmp(node->lable, "StatementList") == 0) {
        for (int i = 0; i < node->childCount; i++) {
            gen_statment(node->children[i], stringTable);
        }
    }
}

/// Functions that handle assignment code generation

void handle_assign(SymbolEntry* entry, int r1) {
    if (entry->place == IS_GLOBAL)
        insert_line("mov [%s], %s\n", entry->name, scratch_name(r1));
    else
        insert_line("mov [rbp + %d], %s\n", entry->offset, scratch_name(r1));
}

void handle_add_assign(SymbolEntry* entry, int r1) {
    if (entry->place == IS_GLOBAL)
        insert_line("add [%s], %s\n", entry->name, scratch_name(r1));
    else
        insert_line("add [rbp + %d], %s\n", entry->offset, scratch_name(r1));
}

void handle_sub_assign(SymbolEntry* entry, int r1) {
    if (entry->place == IS_GLOBAL)
        insert_line("sub [%s], %s\n", entry->name, scratch_name(r1));
    else
        insert_line("sub [rbp + %d], %s\n", entry->offset, scratch_name(r1));
}

void handle_mul_assign(SymbolEntry* entry, int r1) {
    if (entry->place == IS_GLOBAL)
        insert_line("imul qword ptr [%s], %s\n", entry->name, scratch_name(r1));
    else
        insert_line("imul qword ptr [rbp + %d], %s\n", entry->offset, scratch_name(r1));
}

void handle_div_assign(SymbolEntry* entry, int r1) {
    if (entry->place == IS_GLOBAL)
        insert_line("mov rax, [%s]\n", entry->name);
    else
        insert_line("mov rax, [rbp + %d]\n", entry->offset);
    insert_line("cqo\n");
    insert_line("idiv %s\n", scratch_name(r1));
    if (entry->place == IS_GLOBAL)
        insert_line("mov [%s], rax\n", entry->name);
    else
        insert_line("mov [rbp + %d], rax\n", entry->offset);
}

/// <summary>
/// This function handles output statement code generation
/// </summary>
/// <param name="node"></param>
/// <param name="stringTable"></param>
static void gen_output_call(ASTNode* node, HashMap* stringTable)
{    
    // gen expr for child node
    gen_expr(node->children[1], stringTable);
    // get the reg
    int reg = node->children[1]->reg;
    // move the val into rcx
    Type exprType = checkExprType(node->children[1]);
    gen_caller_pushes();
    insert_line("mov rdx, %s\n", scratch_name(reg));
    // call the correct print func based on type
    if (exprType == TYPE_INT) 
    {
        insert_line("call print_int\n");
    }
    else if (exprType == TYPE_BOOL)
    {
        insert_line("call print_bool\n");
    }
    else {
        insert_line("call print_string\n");
    }
    // print a new line
    insert_line("lea rdx, new_line\n");
    insert_line("call print_string\n");
    gen_caller_pops();
}

/// <summary>
/// This function generates input code 
/// </summary>
/// <param name="node"></param>
/// <param name="stringTable"></param>
static void gen_input_call(ASTNode* node, HashMap* stringTable)
{
    // gen expr for child node
    gen_expr(node->children[1], stringTable);
    // get the reg
    int reg = node->children[1]->reg;
    // get the var type
    Type exprType = checkExprType(node->children[1]);
    SymbolTable* scope = getClosestScope(node);
    SymbolEntry* entry = lookUpSymbol(node->children[1]->token->lexeme, scope);
    gen_caller_pushes();
    switch (exprType)
    {
    case TYPE_INT:
        insert_line("call input_int");
        if (entry->place == IS_GLOBAL) {
            insert_line("mov [%s], rax", symbol_codegen(entry));
        }
        else {
            // local
            insert_line("mov %s, rax", symbol_codegen(entry));

        }
        break;
    case TYPE_BOOL:
        insert_line("call input_bool");
        if (entry->place == IS_GLOBAL) {
            insert_line("mov [%s], rax", symbol_codegen(entry));
        }
        else {
            // local
            insert_line("mov %s, rax", symbol_codegen(entry));

        }
        break;
    case TYPE_STRING:   
        // for strings, if the var is global then the buffer is at offset name_buffer if it is a local then the buffer 
        // is on the stack at the offset + 8 bytes if it is a func param the offset is in the reg
        if (entry->place == IS_GLOBAL) 
        {
            // global
            insert_line("lea rdi, %s_buffer", symbol_codegen(entry));
            insert_line("mov rcx, 64");
            insert_line("call input_string");
            // change the pointer of the global var
            insert_line("lea rax, %s_buffer", symbol_codegen(entry));
            insert_line("mov [%s], rax", symbol_codegen(entry));
        }
        else if(entry->place == IS_LOCAL)
        {
            // local
            insert_line("lea rdi, [rbp + %d]", entry->offset + 8);
            insert_line("mov rcx, 64");
            insert_line("call input_string");
            int mem_reg = scratch_alloc();
            // set the pointer to the correct mem addr
            insert_line("lea %s, [rbp + %d]", scratch_name(mem_reg), entry->offset + 8);
            insert_line("mov [rbp + %d], %s", entry->offset, scratch_name(mem_reg));
            scratch_free(mem_reg);
        }
        
        break;
    }
    gen_caller_pops();
}