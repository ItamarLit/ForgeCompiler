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

void gen_function(ASTNode* node, HashMap* stringTable);
void gen_readOnly_data(HashMap* stringTable);
void gen_print_bool();
void gen_print_string();
void gen_print_int();
void gen_str_len();

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
                // strings will refrence the string in the read only segment
                insert_line("%s dq offset %s\n",name,  lookUpString(node->children[3]->token->lexeme, stringTable));
                break;
            case TYPE_BOOL:
                insert_line("%s db %s\n", name, strcmp(node->children[3]->token->lexeme, "true")  == 0 ? "1" : "0");
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

static int count_local_vars(ASTNode* node)
{
    if (!node) return 0;
    int counter = 0;
    // count local var
    if (strcmp(node->lable, "VarDeclaration") == 0)
        counter++;
    // go over all children
    for (int i = 0; i < node->childCount; i++) {
        counter += count_local_vars(node->children[i]);
    }
    return counter;
}

void gen_function(ASTNode* node, HashMap* stringTable)
{
    if (!node) return;
    // get func name
    const char* funcName = node->children[0]->token->lexeme;
    // start of func
    insert_line("%s Proc\n", funcName);
    insert_line("push rbp\n");
    insert_line("mov rbp, rsp\n");
    ASTNode* blockNode = node->children[3];
    // count the amount of local vars in the func and save space for them
    int localSize = count_local_vars(node) * 8;
    insert_line("sub rsp, %d\n", localSize);
    // gen func body
    gen_statement_list(blockNode->children[0], stringTable);
    // setup return label
    insert_line("_%s_ret:\n", funcName);
    // end of func
    insert_line("add rsp, %d\n", localSize);
    insert_line("pop rbp\n");
    insert_line("ret\n");
    insert_line("%s Endp\n", funcName);
}

void gen_winApi()
{
    insert_line("extern ExitProcess: proc\n");
    insert_line("extern GetStdHandle: proc\n");
    insert_line("extern WriteConsoleA: proc\n");
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
    // gen the functions used in output
    gen_str_len();
    gen_print_string();
    gen_print_bool();
    gen_print_int();
    gen_code(root, stringTable);
    insert_line("END\n");

}

/// <summary>
/// This function is used for outputing string values rcx will hold the offset to the string
/// </summary>
void gen_print_string()
{
    insert_line("print_string proc\n");
    insert_line("sub rsp,28h\n");
    insert_line("mov rcx, -11\n");
    insert_line("call GetStdHandle\n");
    insert_line("mov rcx, rax");
    insert_line("call str_len\n"); // rdx = string
    insert_line("lea r9, bytes_written\n");
    insert_line("call WriteConsoleA\n");
    insert_line("add rsp,28h\n");
    insert_line("ret\n");
    insert_line("print_string endp\n");
}

/// <summary>
/// This func is used to print bool values
/// </summary>
void gen_print_bool()
{
    insert_line("print_bool proc\n");
    insert_line("sub rsp,28h\n");
    insert_line("cmp rdx, 0\n");
    insert_line("je _print_false\n");
    insert_line("lea rdx, true_str\n");
    insert_line("jmp _print_done\n");
    insert_line("_print_false:\n");
    insert_line("lea rdx, false_str\n");
    insert_line("_print_done:\n");
    insert_line("mov rcx, -11\n");
    insert_line("call GetStdHandle\n");
    insert_line("call print_string\n");
    insert_line("add rsp,28h\n");
    insert_line("ret\n");
    insert_line("print_bool endp\n");
}

/// <summary>
/// This func is used to print int values
/// </summary>
void gen_print_int()
{
    insert_line("print_int proc\n");
    insert_line("sub rsp,28h\n");
    insert_line("mov rax, rdx\n"); // input number
    insert_line("mov rbx, 10\n");
    insert_line("lea rdi, number_buffer + 20\n");
    insert_line("mov byte ptr [rdi], 0\n");
    insert_line("conv_loop:\n");
    insert_line("xor rdx, rdx\n");
    insert_line("div rbx\n");
    insert_line("add dl, '0'\n");
    insert_line("dec rdi\n");
    insert_line("mov [rdi], dl\n");
    insert_line("test rax, rax\n");
    insert_line("jnz conv_loop\n");
    insert_line("lea rdx, [rdi]\n");
    insert_line("mov rcx, -11\n");
    insert_line("call GetStdHandle\n");
    insert_line("call print_string\n");
    insert_line("add rsp,28h\n");
    insert_line("ret\n");
    insert_line("print_int endp\n");
}
/// <summary>
/// This func will gen an str len function
/// </summary>
void gen_str_len()
{
    insert_line("str_len proc\n");
    insert_line("sub rsp,28h\n");
    insert_line("xor r8d, r8d\n");
    insert_line("len_loop:\n");
    insert_line("cmp byte ptr [rdx + r8], 0\n");
    insert_line("je len_done\n");
    insert_line("inc r8d\n");
    insert_line("jmp len_loop\n");
    insert_line("len_done:\n");
    insert_line("add rsp, 28h\n");
    insert_line("ret\n");
    insert_line("str_len endp\n");
}