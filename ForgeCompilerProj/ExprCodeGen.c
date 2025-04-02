#pragma warning (disable:4996)
#include "ExprCodeGen.h"
#include "CodeGenUtils.h"
#include "StringTable.h"
#include <stdio.h>
#include <string.h>

static void gen_int_literal(ASTNode* node, HashMap* stringTable);
static void gen_bool_true(ASTNode* node, HashMap* stringTable);
static void gen_bool_false(ASTNode* node, HashMap* stringTable);
static void gen_string_literal(ASTNode* node, HashMap* stringTable);
static void gen_identifier(ASTNode* node, HashMap* stringTable);
static void gen_unary_expr(ASTNode* node, HashMap* stringTable);
static void gen_binary_expr(ASTNode* node, HashMap* stringTable);
static void gen_logical_and(ASTNode* node, HashMap* stringTable);
static void gen_logical_or(ASTNode* node, HashMap* stringTable);
static void gen_func_call_expr(ASTNode* node, HashMap* stringTable);
static void gen_equality_expr(ASTNode* node, HashMap* stringTable);
static void gen_relational_expr(ASTNode* node, HashMap* stringTable);

static HandlerEntry exprTable[] =
{
    {"INT_LITERAL", gen_int_literal},
    {"true", gen_bool_true},
    {"false", gen_bool_false},
    {"STRING_LITERAL", gen_string_literal},
    {"IDENTIFIER", gen_identifier},
    {"UnaryExpr", gen_unary_expr},
    {"AddExpr", gen_binary_expr},
    {"MulExpr", gen_binary_expr},
    {"AndExpr", gen_logical_and},
    {"OrExpr", gen_logical_or},
    {"FuncCallExpr", gen_func_call_expr},
    {"EqualityExpr", gen_equality_expr},
    {"RelationalExpr", gen_relational_expr},
    {NULL, NULL}
};


void gen_expr(ASTNode* node, HashMap* stringTable)
{
    if (!node) return;
    if (strcmp(node->lable, "Expr") == 0) {
        gen_expr(node->children[0], stringTable);
        return;
    }
    // go over all labels and check what func to run
    for (int i = 0; exprTable[i].label != NULL; i++) {
        if (strcmp(node->lable, exprTable[i].label) == 0) {
            exprTable[i].handler(node, stringTable);
            return;
        }
    }
    
}


static void gen_int_literal(ASTNode* node, HashMap* stringTable)
{
    int r = scratch_alloc();
    insert_line("mov %s, %s\n", scratch_name(r), node->token->lexeme);
    // save the used reg
    node->reg = r;
}

static void gen_bool_true(ASTNode* node, HashMap* stringTable) 
{
    int r = scratch_alloc();
    insert_line("mov %s, 1\n", scratch_name(r));
    node->reg = r;
}

static void gen_bool_false(ASTNode* node, HashMap* stringTable)
{
    int r = scratch_alloc();
    insert_line("mov %s, 0\n", scratch_name(r));
    node->reg = r;
}

static void gen_string_literal(ASTNode* node, HashMap* stringTable) 
{
    // get the offset label from the read only segment
    int r = scratch_alloc();
    insert_line("mov %s, offset %s\n", scratch_name(r), lookUpString(node->token->lexeme, stringTable));
    node->reg = r;
}

static void gen_identifier(ASTNode* node, HashMap* stringTable) 
{
    int r = scratch_alloc();
    SymbolTable* scope = getClosestScope(node);
    SymbolEntry* sym = lookUpSymbol(node->token->lexeme, scope);
    insert_line("mov %s, %s\n", scratch_name(r), symbol_codegen(sym));
    node->reg = r;
}

static void gen_unary_expr(ASTNode* node, HashMap* stringTable) 
{
    // gen code only for - expr
    if (node->childCount == 2 && strcmp(node->children[0]->lable, "-") == 0) 
    {
        // gen the code for the child
        gen_expr(node->children[1], stringTable);
        // get the child reg
        int r = node->children[1]->reg;
        // neg it
        insert_line("neg %s\n", scratch_name(r));
        node->reg = r;
    }
}

static void gen_binary_expr(ASTNode* node, HashMap* stringTable)
{
    BinEntry opTable[] =
    {
        {"+", "add"},
        {"-", "sub"},
        {"*", "imul"},
        {NULL, NULL}
    };
    // gen left
    gen_expr(node->children[0], stringTable);
    // gen right
    gen_expr(node->children[2], stringTable);
    // get the registers
    int r1 = node->children[0]->reg;
    int r2 = node->children[2]->reg;
    // get the asm op
    const char* op = NULL;
    // handle div seperatly
    if (strcmp(node->children[1]->lable, "/") == 0) 
    {
        // handle the div
        insert_line("mov rax, %s\n", scratch_name(r1));
        insert_line("cqo\n");
        insert_line("idiv %s\n", scratch_name(r2));
        insert_line("mov %s, rax\n", scratch_name(r1));
        scratch_free(r2);
        node->reg = r1;
        return;
    }
    for (int i = 0; opTable[i].label != NULL; i++) {
        if (strcmp(node->children[1]->lable, opTable[i].label) == 0) {
            op = opTable[i].op;
        }
    }
    // output asm 
    insert_line("%s %s, %s\n", op, scratch_name(r1), scratch_name(r2));
    // free the right side register
    scratch_free(r2);
    // set the res in the correct reg
    node->reg = r1;
}

void gen_logical_and(ASTNode* node, HashMap* stringTable)
{
    // gen the left expr
    gen_expr(node->children[0], stringTable);
    int leftReg = node->children[0]->reg;
    // create labels
    char* skipRight = label_name();
    char* done = label_name();
    // cmp to false
    insert_line("cmp %s, 0\n", scratch_name(leftReg));
    // if a false the whole thing is false
    insert_line("je %s\n", skipRight);
    // if a not false
    scratch_free(leftReg);
    // gen right expr
    gen_expr(node->children[2], stringTable);
    int rightReg = node->children[2]->reg;
    // move result from the right to the res reg
    int result = scratch_alloc();
    insert_line("mov %s, %s\n", scratch_name(result), scratch_name(rightReg));
    // free right reg
    scratch_free(rightReg);
    insert_line("jmp %s\n", done);
    // skip label
    insert_line("%s:\n", skipRight);
    // move res reg 0
    insert_line("mov %s, 0\n", scratch_name(result));
    // done label
    insert_line("%s:\n", done);
    free(done);
    free(skipRight);
    // left reg has output
    node->reg = result;
}


void gen_logical_or(ASTNode* node, HashMap* stringTable)
{
    // gen left expr
    gen_expr(node->children[0], stringTable);
    int leftReg = node->children[0]->reg;
    // gen labels
    char* skipRight = label_name();
    char* done = label_name();
    // if left is true skip right
    insert_line("cmp %s, 0\n", scratch_name(leftReg));
    insert_line("jne %s\n", skipRight);
    // free reg left was false
    scratch_free(leftReg);
    // gen right
    gen_expr(node->children[2], stringTable);
    int rightReg = node->children[2]->reg;
    // move right into res reg
    int result = scratch_alloc();
    insert_line("mov %s, %s\n", scratch_name(result), scratch_name(rightReg));
    scratch_free(rightReg);
    // left holds the result
    insert_line("jmp %s\n", done);
    // skip label
    insert_line("%s:\n", skipRight);
    insert_line("mov %s, 1\n", scratch_name(result));
    // done label
    insert_line("%s:\n", done);
    free(done);
    free(skipRight);
    // left reg has result
    node->reg = result;
}

static void gen_func_call_expr(ASTNode* node, HashMap* stringTable)
{
    const char* funcName = node->children[0]->token->lexeme;
    // get the arg count
    ASTNode* argsNode = node->children[1];
    int argCount = argsNode->childCount;
    // these are the 4 var reg by MASM conventions
    static const char* regArg[4] = { "rcx", "rdx", "r8", "r9" };
    // get the number of stack vars
    int numStackArgs = (argCount > 4) ? (argCount - 4) : 0;
    // calc the amount of stack space needed
    int stackSpace = 8 * numStackArgs;
    // remove space for stack vars and shadow space for reg vars
    insert_line("sub rsp, %d\n", stackSpace + 32);
    // assign the registers there values
    int regIndex = 0;
    int i;
    for (i = 0; i < argCount; i++) {
        gen_expr(argsNode->children[i], stringTable);
        // get the reg the var is stored in currently
        int argReg = argsNode->children[i]->reg;
        // move the value from the scratch reg to the correct reg
        if (regIndex < 4) {
            insert_line("mov %s, %s\n", regArg[regIndex], scratch_name(argReg));
            regIndex++;
            scratch_free(argReg);
        }
    }
    // push stack args in rev
    for (i = argCount; i > 4; i--) {
        int argReg = argsNode->children[i]->reg;
        insert_line("mov rax, %s\n", scratch_name(argReg));
        insert_line("push rax\n");
        scratch_free(argReg);
    }
    // before calling the func push any volitile reg or used scratch reg

    insert_line("push rcx\n");
    insert_line("push rdx\n");
    insert_line("push r8\n");
    insert_line("push r9\n");

    insert_line("push rbx\n");
    insert_line("push r10\n");
    insert_line("push r11\n");
    insert_line("push r12\n");
    insert_line("push r13\n");
    insert_line("push r14\n");
    insert_line("push r15\n");
    // call the func
    insert_line("call %s\n", funcName);
    // after call pop all the prev pushed reg
    insert_line("pop r15\n");
    insert_line("pop r14\n");
    insert_line("pop r13\n");
    insert_line("pop r12\n");
    insert_line("pop r11\n");
    insert_line("pop r10\n");
    insert_line("pop rbx\n");

    insert_line("pop r9\n");
    insert_line("pop r8\n");
    insert_line("pop rdx\n");
    insert_line("pop rcx\n");

    // store the ret value in a scratch register
    int r = scratch_alloc();
    insert_line("mov %s, rax\n", scratch_name(r));
    node->reg = r;
    // clean stack
    insert_line("add rsp, %d\n", stackSpace + 32);
}

static void gen_equality_expr(ASTNode* node, HashMap* stringTable) 
{
    gen_expr(node->children[0], stringTable);
    gen_expr(node->children[2], stringTable);
    // get the registers of both children
    int r1 = node->children[0]->reg;
    int r2 = node->children[2]->reg;
    // compare the reg
    insert_line("cmp %s, %s\n", scratch_name(r1), scratch_name(r2));
    // get two labels
    char* trueLabel = label_name();
    char* doneLabel = label_name();
    // if we are checking equal
    if (strcmp(node->children[1]->lable, "==") == 0) {
        insert_line("je %s\n", trueLabel);
    }
    else {
        // we are checking not equal
        insert_line("jne %s\n", trueLabel);
    }
    // if we dont take the jmp then we are false
    insert_line("mov %s, 0\n", scratch_name(r1));
    insert_line("jmp %s\n", doneLabel);
    // true label set true
    insert_line("%s:\n", trueLabel);
    free(trueLabel);
    insert_line("mov %s, 1\n", scratch_name(r1));
    // done
    insert_line("%s:\n", doneLabel);
    free(doneLabel);
    // free the second reg
    scratch_free(r2);
    node->reg = r1;
}

static void gen_relational_expr(ASTNode* node, HashMap* stringTable)
{
    gen_expr(node->children[0], stringTable);
    gen_expr(node->children[2], stringTable);
    // get the two children registers
    int r1 = node->children[0]->reg;
    int r2 = node->children[2]->reg;
    // compare
    insert_line("cmp %s, %s\n", scratch_name(r1), scratch_name(r2));
    // get two labels
    char* trueLabel = label_name();
    char* doneLabel = label_name();
    // get the op child
    const char* op = node->children[1]->token->lexeme;
    // set the correct op
    if (strcmp(op, ">") == 0) {
        insert_line("jg %s\n", trueLabel);
    }
    else if (strcmp(op, ">=") == 0) {
        insert_line("jge %s\n", trueLabel);
    }
    else if (strcmp(op, "<") == 0) {
        insert_line("jl %s\n", trueLabel);
    }
    else if (strcmp(op, "<=") == 0) {
        insert_line("jle %s\n", trueLabel);
    }
    // false path
    insert_line("mov %s, 0\n", scratch_name(r1));
    insert_line("jmp %s\n", doneLabel);
    // true path
    insert_line("%s:\n", trueLabel);
    free(trueLabel);
    insert_line("mov %s, 1\n", scratch_name(r1));
    // done
    insert_line("%s:\n", doneLabel);
    free(doneLabel);
    // free the second reg
    scratch_free(r2);
    node->reg = r1;
}