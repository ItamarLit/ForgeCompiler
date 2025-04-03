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
void gen_input_string();
void gen_input_int();
void gen_input_bool();
void gen_invalid_input();
void gen_buffer_overflow();

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
    gen_str_len();
    gen_print_string();
    gen_print_bool();
    gen_print_int();
    gen_input_string();
    gen_input_int();
    gen_input_bool();
    gen_invalid_input();
    gen_buffer_overflow();
    gen_code(root, stringTable);
    insert_line("END\n");

}

/// <summary>
/// This function is used for outputing string values rcx will hold the offset to the string
/// </summary>
void gen_print_string()
{
    insert_line("print_string proc\n");
    // func setup
    insert_line("sub rsp,20h\n");
    // get the output handle
    insert_line("mov rcx, -11\n");

    insert_line("call GetStdHandle\n");

    insert_line("mov rcx, rax");
    // get the str len dont push reg, we need r8
    insert_line("call str_len\n"); 

    insert_line("lea r9, bytes_written\n");
    // write to console

    insert_line("call WriteConsoleA\n");

    // func end
    insert_line("add rsp,20h\n");
    insert_line("ret\n");
    insert_line("print_string endp\n");
}

/// <summary>
/// This func is used to print bool values, it uses the print string func rdx will hold the value (0 = false, 1 = true)
/// </summary>
void gen_print_bool()
{
    insert_line("print_bool proc\n");
    // func setup
    insert_line("sub rsp,20h\n");
    // check if it is false
    insert_line("cmp rdx, 0\n");
    insert_line("je _print_false\n");
    // print true
    insert_line("lea rdx, true_str\n");
    insert_line("jmp _print_done\n");
    // print false
    insert_line("_print_false:\n");
    insert_line("lea rdx, false_str\n");
    insert_line("_print_done:\n");
    // call print string
    insert_line("call print_string\n");

    // func end
    insert_line("add rsp,20h\n");
    insert_line("ret\n");
    insert_line("print_bool endp\n");
}

/// <summary>
/// This func is used to print int values, rdx holds the input number
/// </summary>
void gen_print_int()
{
    insert_line("print_int proc");
    // func setup
    insert_line("sub rsp,20h");
    // rax will hold the number
    insert_line("mov rax, rdx"); 
    insert_line("mov rbx, 10");
    // check if the num is neg
    insert_line("cmp rax, 0");
    // if not neg can jump to conversion
    insert_line("jge conv_start"); 
    // make rax positive
    insert_line("neg rax");         
    // store the '-' sign in the first byte of buffer
    insert_line("mov byte ptr [number_buffer], '-'"); 
    insert_line("conv_start:");
    // go to the end (build number in rev)
    insert_line("lea rdi, number_buffer + 20");
    // set up null terminator
    insert_line("mov byte ptr [rdi], 0");
    insert_line("dec rdi");
    // int conversion loop
    insert_line("conv_loop:");
    insert_line("xor rdx, rdx");
    insert_line("div rbx");
    // add 0 to make it a char
    insert_line("add dl, '0'");
    insert_line("dec rdi");
    // store the char in the buffer
    insert_line("mov [rdi], dl");
    // if rax != 0 keep going
    insert_line("test rax, rax");
    insert_line("jnz conv_loop");
    // check if the num was negative
    insert_line("cmp byte ptr [number_buffer], '-'");
    insert_line("jne skip_sign");
    // place a '-' at the real start of the number
    insert_line("dec rdi");
    insert_line("mov al, '-'");
    insert_line("mov [rdi], al");
    insert_line("skip_sign:");
    // print result
    insert_line("lea rdx, [rdi]");
    insert_line("call print_string");

    // func end
    insert_line("add rsp,20h");
    insert_line("ret");
    insert_line("print_int endp");
}
/// <summary>
/// This func will gen an str len function the output (len) is stored in r8
/// </summary>
void gen_str_len()
{
    insert_line("str_len proc\n");
    // func setup
    insert_line("sub rsp,20h\n");
    // r8 will hold the output
    insert_line("xor r8d, r8d\n");
    // loop until hit 0 (end of string)
    insert_line("len_loop:\n");
    insert_line("cmp byte ptr [rdx + r8], 0\n");
    insert_line("je len_done\n");
    // inc counter
    insert_line("inc r8d\n");
    insert_line("jmp len_loop\n");
    insert_line("len_done:\n");
    // func end
    insert_line("add rsp, 20h\n");
    insert_line("ret\n");
    insert_line("str_len endp\n");
}

/// <summary>
/// This func will gen a function that gets input and saves it in a buffer the buffer is in rdi
/// </summary>
void gen_input_string()
{
    insert_line("input_string proc");
    // func start
    insert_line("sub rsp,20h\n");
    // get input handle
    insert_line("mov rcx, -10");
    insert_line("call GetStdHandle");
    // save handle in r8
    insert_line("mov r8, rax");          
    // handle var
    insert_line("mov rcx, r8");          
    // buffer addr
    insert_line("mov rdx, rdi");        
    // input size
    insert_line("mov r8d, 64");          
    // room for null terminator
    insert_line("dec r8d");              
    // bytes red
    insert_line("lea r9, bytes_read");   
    insert_line("xor rax, rax");        
    // get the input
    insert_line("call ReadConsoleA");
    insert_line("mov rax, bytes_read");

    insert_line("cmp rax, 63");
    insert_line("jb skip_error");
    insert_line("call buffer_overflow");
    insert_line("skip_error:");


    // null terminate the input
    insert_line("mov rax, bytes_read");
    insert_line("cmp rax, 0");
    // nothing was read
    insert_line("je input_done");
    // check if the last two bytes are carrige return
    insert_line("mov bl, byte ptr [rdi + rax - 2]");
    insert_line("cmp bl, 13");                       
    insert_line("jne no_carrige");
    // null terminate the return 
    insert_line("mov byte ptr [rdi + rax - 2], 0");  
    insert_line("jmp input_done");
    insert_line("no_carrige:");
    // null terminate the end
    insert_line("mov byte ptr [rdi + rax], 0");      
    insert_line("input_done:");
    // func end
    insert_line("add rsp,20h");
    insert_line("ret");
    insert_line("input_string endp");
}

/// <summary>
/// This func generates a function that handles int value input, if an invalid value is entered it closes the program
/// </summary>
void gen_input_int()
{
    insert_line("input_int proc");
    // func start
    insert_line("sub rsp,20h\n");
    // create buffer on the stack 
    insert_line("sub rsp, 64");
    // rdi points to the buffer
    insert_line("lea rdi, [rsp]");   
    // get the input
    insert_line("call input_string");

    // init registers
    insert_line("xor rax, rax");    
    // index
    insert_line("xor rcx, rcx");    
    // negative flag
    insert_line("mov r8b, 0");      
    // check for empty input
    insert_line("movzx rdx, byte ptr [rdi]");
    insert_line("cmp rdx, 0");
    insert_line("je invalid_input");
    // check for negative input
    insert_line("cmp rdx, '-'");
    insert_line("jne parse_loop");
    // mark in reg and skip '-'
    insert_line("mov r8b, 1");      
    insert_line("inc rcx");         
    // prevent only '-'
    insert_line("movzx rdx, byte ptr [rdi + rcx]");
    insert_line("cmp rdx, 0");
    insert_line("je invalid_input");
    // main parse loop
    insert_line("parse_loop:");
    insert_line("movzx rdx, byte ptr [rdi + rcx]");
    // check if end
    insert_line("cmp rdx, 0");
    insert_line("je parse_done");   
    // validate digit
    insert_line("cmp rdx, '0'");
    insert_line("jl invalid_input");
    insert_line("cmp rdx, '9'");
    insert_line("jg invalid_input");
    // convert to int
    insert_line("sub rdx, '0'");
    insert_line("imul rax, 10");
    insert_line("add rax, rdx");
    insert_line("inc rcx");
    insert_line("jmp parse_loop");
    // apply neg value if needed
    insert_line("parse_done:");
    insert_line("cmp r8b, 0");
    insert_line("je parse_end");
    insert_line("neg rax");
    // func end
    insert_line("parse_end:");
    insert_line("add rsp, 64");
    insert_line("add rsp,20h");
    insert_line("ret");
    insert_line("input_int endp");
}

/// <summary>
/// This function will generate a bool parse func based on true/ false input, if input is invalid the func will catch it
/// </summary>
void gen_input_bool()
{
    insert_line("input_bool proc");
    // func start
    insert_line("sub rsp,20h\n");
    // allocate buffer on stack
    insert_line("sub rsp, 64");
    insert_line("lea rdi, [rsp]");
    // call input string
    insert_line("call input_string");
    // check if input is true, eax flag
    insert_line("mov eax, 1");              
    // load first 4 chars
    insert_line("mov r8d, DWORD PTR [rdi]"); 
    // compare to true, reversed
    insert_line("cmp r8d, 'eurt'");          
    insert_line("jne is_false");
    insert_line("movzx r9, byte ptr [rdi + 4]");
    // check null terminator
    insert_line("cmp r9, 0");               
    insert_line("je bool_done");
    // check if it is false
    insert_line("is_false:");
    insert_line("xor eax, eax");            
    // check with false str
    insert_line("mov r8, QWORD PTR [rdi]");            
    insert_line("cmp r8, QWORD PTR false_str");         
    insert_line("je bool_done");
    // if not false input is invalid
    insert_line("call invalid_input");
    insert_line("bool_done:");
    // func end
    insert_line("add rsp, 64");             
    insert_line("add rsp,20h");
    insert_line("ret");
    insert_line("input_bool endp");
}

/// <summary>
/// This function is used to tell the user that the input is invalid
/// </summary>
void gen_invalid_input() 
{
    insert_line("invalid_input proc");
    insert_line("sub rsp,20h");
    // print the msg
    insert_line("lea rdx, new_line\n");
    insert_line("call print_string\n");
    insert_line("lea rdx, invalid_input_str\n");
    insert_line("call print_string\n");
    // exit with error code
    insert_line("mov ecx, 1");
    insert_line("call ExitProcess");
    insert_line("invalid_input endp");

}

/// <summary>
/// This func is used to show buffer overflow error
/// </summary>
void gen_buffer_overflow()
{
    insert_line("buffer_overflow proc");
    insert_line("sub rsp,20h");
    // print the msg
    insert_line("lea rdx, new_line\n");
    insert_line("call print_string\n");
    insert_line("lea rdx, buffer_overflow_str\n");
    insert_line("call print_string\n");
    // exit with error code
    insert_line("mov ecx, 1");
    insert_line("call ExitProcess");
    insert_line("buffer_overflow endp");
}