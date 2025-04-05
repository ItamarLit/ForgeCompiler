#pragma warning (disable:4996)
#include <stdio.h>
#include <string.h>
#include "Types.h"
#include "ExprCodeGen.h"
#include "SymbolTable.h"
#include "StmtCodeGen.h"
#include "CodeGenerator.h"
#include "HashMap.h"
#include "StringTable.h"
#include "CodeGenUtils.h"
#include "HelperFunctionsCodeGen.h"

void gen_function(ASTNode* node, HashMap* stringTable);
void gen_readOnly_data(HashMap* stringTable);


/// <summary>
/// This function will go over the root and generate the global variables
/// </summary>
/// <param name="root"></param>
void gen_data_seg(ASTNode* root, HashMap* stringTable)
{
	// gen the lable
    insert_line(".data\n");
    // gen read only vars
    gen_readOnly_data(stringTable);
    if (!root) return;
    // get the global symb table
    SymbolTable* scope = root->scope;
    // go over all global nodes
    for (int i = 0; i < root->childCount; i++) {
        ASTNode* node = root->children[i];
        // look for global var dec
        if (node->lable && strcmp(node->lable, "VarDeclaration") == 0) {
            ASTNode* nameNode = node->children[1];
            char* name = nameNode->token->lexeme;
            // get the symbol table entry
            SymbolEntry* entry = lookUpSymbol(name, scope);
            switch (entry->type)
            {
            case TYPE_INT:
                insert_line("%s dq %s\n",name, node->children[3]->token->lexeme);
                break;
            case TYPE_STRING:
                // strings will refrence the string in the read only seg originaly but creat buffer if need to change them
                insert_line("%s_buffer db 64 dup(0)\n", name);
                insert_line("%s dq offset %s\n",name, lookUpString(node->children[3]->token->lexeme, stringTable));
                break;
            case TYPE_BOOL:
                insert_line("%s dq %s\n", name, strcmp(node->children[3]->token->lexeme, "true")  == 0 ? "1" : "0");
                break;
            default:
                insert_line("Invalid global var type: %s\n", convertTypeToString(entry->type));
                break;
            }
        }
    }

}

void gen_readOnly_data(HashMap* stringTable) {
    int index;
    for (index = 0; index < stringTable->map_size; index++) {
        if (stringTable->arr[index] != NULL) {
            HashNode* cur = stringTable->arr[index];
            while (cur != NULL) {
                insert_line("%s db %s,0\n",((StringEntry*)cur->value)->label, (char*)cur->key);
                cur = cur->next;
            }
        }
    }
    // add the strings used in output and input
    insert_line("true_str db %s,0\n", "\"true\"");
    insert_line("false_str db %s,0\n", "\"false\"");
    insert_line("new_line db 13,10,0\n");
    insert_line("number_buffer db 21 dup(0)\n");
    insert_line("bytes_written dd 0\n");
    insert_line("bytes_read dq 0\n"); 
    insert_line("invalid_input_str db \"The input type did not match, goodbye!\",0\n");
    insert_line("buffer_overflow_str db \"Buffer overflow, max input 63 chars, goodbye!\",0\n");

}



/// <summary>
/// This is the main code gen function, it will preform a Postorder traversel
/// </summary>
/// <param name="node"></param>
void gen_code(ASTNode* node, HashMap* stringTable)
{
    if (!node) return;
    // go over children
    for (int i = 0; i < node->childCount; i++) {
        gen_code(node->children[i], stringTable); 
    }
    if (strcmp(node->lable, "FuncDeclaration") == 0) {
        gen_function(node, stringTable);
        return;
    }
}

static int count_local_var_bytes(ASTNode* node)
{
    if (!node) return 0;
    int counter = 0;
    if (strcmp(node->lable, "VarDeclaration") == 0) {
        Type varType = checkExprType(node->children[1]);  
        if (varType == TYPE_STRING)
            counter += 72;   
        else
            counter += 8;    
    }

    // recurse through children
    for (int i = 0; i < node->childCount; i++) {
        counter += count_local_var_bytes(node->children[i]);
    }

    return counter;
}

int gen_stack_allocation(int local_bytes)
{
    int total_bytes = local_bytes;
    int rsp_after_push = total_bytes;
    // allign stack
    if (rsp_after_push % 16 != 0)
    {
        int padding = 16 - (rsp_after_push % 16);
        total_bytes += padding;
    }
    return total_bytes;
}


void gen_function(ASTNode* node, HashMap* stringTable)
{
    if (!node) return;
    // get func name
    const char* funcName = node->children[0]->token->lexeme;
    // start of func
    insert_line("%s Proc\n", funcName);
    if (strcmp(funcName, "Main") == 0)
    {
        insert_line("sub rsp, 8");
    }
    insert_line("push rbp\n");
    insert_line("mov rbp, rsp\n");
    // push callee saved registers
    gen_callee_pushes();
    ASTNode* blockNode = node->children[3];
    // count the amount of local vars in the func and save space for them
    int localSize = count_local_var_bytes(node);
    insert_line("sub rsp, %d\n", gen_stack_allocation(localSize));
    // gen func body
    gen_statement_list(blockNode->children[0], stringTable);
    // setup return label
    insert_line("_%s_ret:\n", funcName);
    insert_line("add rsp, %d\n", gen_stack_allocation(localSize));
    gen_callee_pops();
    insert_line("pop rbp\n");
    if (strcmp(funcName, "Main") == 0) 
    {
        
        insert_line("mov ecx, 0");
        insert_line("call ExitProcess");

    }
    else
    {
        // end of func
        insert_line("ret\n");
    }
    insert_line("%s Endp\n", funcName);
}

void gen_winApi()
{
    insert_line("extern ExitProcess: proc\n");
    insert_line("extern GetStdHandle: proc\n");
    insert_line("extern WriteConsoleA: proc\n");
    insert_line("extern ReadConsoleA : proc\n");
}

void gen_asm(ASTNode* root, HashMap* stringTable) 
{
    createAsmFile();
    // gen the winApi funcs
    gen_winApi();
    // gen data seg
    gen_data_seg(root, stringTable);
    // gen code seg
    insert_line(".code\n");
    // gen the functions used in output and output
    gen_helper_functions();
    gen_code(root, stringTable);
    insert_line("END\n");
}