#include "HelperFunctionsCodeGen.h"

static void gen_print_bool();
static void gen_print_string();
static void gen_print_int();
static void gen_str_len();
static void gen_input_string();
static void gen_input_int();
static void gen_input_bool();
static void gen_invalid_input();
static void gen_buffer_overflow();
static void gen_copy_string();

void gen_helper_functions() 
{
    // gen string len func
    gen_str_len();
    // gen print string
    gen_print_string();
    // gen print bool
    gen_print_bool();
    // gen print int
    gen_print_int();
    // gen input string
    gen_input_string();
    // gen input int
    gen_input_int();
    // gen input bool
    gen_input_bool();
    // gen invalid input func
    gen_invalid_input();
    // gen buff overflow func
    gen_buffer_overflow();
    // gen copy string func
    gen_copy_string();
}

/// <summary>
/// This helper function will copy a string from rsi into rdi
/// </summary>
static void gen_copy_string()
{
    insert_line("copy_string proc\n");
    insert_line("sub rsp,20h\n");
    insert_line("push rcx\n");
    // 8 qwords
    insert_line("mov rcx, 8\n");  
    insert_line("copy_loop:\n");
    // load 8 bytes from src
    insert_line("mov rax, [rsi]\n");   
    // mov 8 bytes to dest
    insert_line("mov [rdi], rax\n");   
    insert_line("add rsi, 8\n");       
    insert_line("add rdi, 8\n");      
    insert_line("loop copy_loop\n");
    // func end
    insert_line("pop rcx\n");
    insert_line("add rsp,20h\n");
    insert_line("ret\n");
    insert_line("copy_string endp\n");
}

/// <summary>
/// This function is used for outputing string values rcx will hold the offset to the string
/// </summary>
static void gen_print_string()
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
static void gen_print_bool()
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
static void gen_print_int()
{
    insert_line("print_int proc\n");
    // func setup
    insert_line("sub rsp,20h\n");
    // rax will hold the number
    insert_line("mov rax, rdx\n");
    insert_line("mov rbx, 10\n");
    // check if the num is neg
    insert_line("cmp rax, 0\n");
    // if not neg can jump to conversion
    insert_line("jge conv_start\n");
    // make rax positive
    insert_line("neg rax\n");
    // store the '-' sign in the first byte of buffer
    insert_line("mov byte ptr [number_buffer], '-'\n");
    insert_line("conv_start:\n");
    // go to the end (build number in rev)
    insert_line("lea rdi, number_buffer + 20\n");
    // set up null terminator
    insert_line("mov byte ptr [rdi], 0\n");
    insert_line("dec rdi\n");
    // int conversion loop
    insert_line("conv_loop:\n");
    insert_line("xor rdx, rdx\n");
    insert_line("div rbx\n");
    // add 0 to make it a char
    insert_line("add dl, '0'\n");
    insert_line("dec rdi\n");
    // store the char in the buffer
    insert_line("mov [rdi], dl\n");
    // if rax != 0 keep going
    insert_line("test rax, rax\n");
    insert_line("jnz conv_loop\n");
    // check if the num was negative
    insert_line("cmp byte ptr [number_buffer], '-'\n");
    insert_line("jne skip_sign\n");
    // place a '-' at the real start of the number
    insert_line("dec rdi\n");
    insert_line("mov al, '-'\n");
    insert_line("mov [rdi], al\n");
    insert_line("skip_sign:\n");
    // print result
    insert_line("lea rdx, [rdi]\n");
    insert_line("call print_string\n");
    // func end
    insert_line("add rsp,20h\n");
    insert_line("ret");
    insert_line("print_int endp\n");
}
/// <summary>
/// This func will gen an str len function the output (len) is stored in r8
/// </summary>
static void gen_str_len()
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
static void gen_input_string()
{
    insert_line("input_string proc\n");
    // func start
    insert_line("sub rsp,20h\n");
    // get input handle
    insert_line("mov rcx, -10\n");
    insert_line("call GetStdHandle\n");
    // save handle in r8
    insert_line("mov r8, rax\n");
    // handle var
    insert_line("mov rcx, r8\n");
    // buffer addr
    insert_line("mov rdx, rdi\n");
    // input size
    insert_line("mov r8d, 64\n");
    // room for null terminator
    insert_line("dec r8d\n");
    // bytes red
    insert_line("lea r9, bytes_read\n");
    insert_line("xor rax, rax\n");
    // get the input
    insert_line("call ReadConsoleA\n");
    insert_line("xor rax, rax\n");
    insert_line("mov eax, bytes_read\n");
    insert_line("cmp rax, 63\n");
    insert_line("jb skip_error\n");
    insert_line("call buffer_overflow\n");
    insert_line("skip_error:\n");
    // null terminate the input
    insert_line("xor rax, rax\n");
    insert_line("mov eax, bytes_read\n");
    insert_line("cmp rax, 0\n");
    // nothing was read
    insert_line("je input_done\n");
    // check if the last two bytes are carrige return
    insert_line("mov bl, byte ptr [rdi + rax - 2]\n");
    insert_line("cmp bl, 13\n");
    insert_line("jne no_carrige\n");
    // null terminate the return 
    insert_line("mov byte ptr [rdi + rax - 2], 0\n");
    insert_line("jmp input_done\n");
    insert_line("no_carrige:\n");
    // null terminate the end
    insert_line("mov byte ptr [rdi + rax], 0\n");
    insert_line("input_done:\n");
    // func end
    insert_line("add rsp,20h\n");
    insert_line("ret\n");
    insert_line("input_string endp\n");
}

/// <summary>
/// This func generates a function that handles int value input, if an invalid value is entered it closes the program
/// </summary>
static void gen_input_int()
{
    insert_line("input_int proc\n");
    // func start
    insert_line("sub rsp,20h\n");
    // create buffer on the stack 
    insert_line("sub rsp, 64\n");
    // rdi points to the buffer
    insert_line("lea rdi, [rsp]\n");
    // get the input
    insert_line("call input_string\n");
    // init registers
    insert_line("xor rax, rax\n");
    // index
    insert_line("xor rcx, rcx\n");
    // negative flag
    insert_line("mov r8b, 0\n");
    // check for empty input
    insert_line("movzx rdx, byte ptr [rdi]\n");
    insert_line("cmp rdx, 0\n");
    insert_line("je invalid_input\n");
    // check for negative input
    insert_line("cmp rdx, '-'\n");
    insert_line("jne parse_loop\n");
    // mark in reg and skip '-'
    insert_line("mov r8b, 1\n");
    insert_line("inc rcx\n");
    // prevent only '-'
    insert_line("movzx rdx, byte ptr [rdi + rcx]\n");
    insert_line("cmp rdx, 0\n");
    insert_line("je invalid_input\n");
    // main parse loop
    insert_line("parse_loop:\n");
    insert_line("movzx rdx, byte ptr [rdi + rcx]\n");
    // check if end
    insert_line("cmp rdx, 0\n");
    insert_line("je parse_done\n");
    // validate digit
    insert_line("cmp rdx, '0'\n");
    insert_line("jl invalid_input\n");
    insert_line("cmp rdx, '9'\n");
    insert_line("jg invalid_input\n");
    // convert to int
    insert_line("sub rdx, '0'\n");
    insert_line("imul rax, 10\n");
    insert_line("add rax, rdx\n");
    insert_line("inc rcx\n");
    insert_line("jmp parse_loop\n");
    // apply neg value if needed
    insert_line("parse_done:\n");
    insert_line("cmp r8b, 0\n");
    insert_line("je parse_end\n");
    insert_line("neg rax\n");
    // func end
    insert_line("parse_end:\n");
    insert_line("add rsp, 64\n");
    insert_line("add rsp,20h\n");
    insert_line("ret\n");
    insert_line("input_int endp\n");
}

/// <summary>
/// This function will generate a bool parse func based on true/ false input, if input is invalid the func will catch it
/// </summary>
static void gen_input_bool()
{
    insert_line("input_bool proc\n");
    // func start
    insert_line("sub rsp,20h\n");
    // allocate buffer on stack
    insert_line("sub rsp, 64\n");
    insert_line("lea rdi, [rsp]\n");
    // call input string
    insert_line("call input_string\n");
    // check if input is true, eax flag
    insert_line("mov eax, 1\n");
    // load first 4 chars
    insert_line("mov r8d, DWORD PTR [rdi]\n");
    // compare to true, reversed
    insert_line("cmp r8d, 'eurt'\n");
    insert_line("jne is_false\n");
    insert_line("movzx r9, byte ptr [rdi + 4]\n");
    // check null terminator
    insert_line("cmp r9, 0\n");
    insert_line("je bool_done\n");
    // check if it is false
    insert_line("is_false:\n");
    insert_line("xor eax, eax\n");
    // check with false str
    insert_line("mov r8, QWORD PTR [rdi]\n");
    insert_line("cmp r8, QWORD PTR false_str\n");
    insert_line("je bool_done\n");
    // if not false input is invalid
    insert_line("call invalid_input\n");
    insert_line("bool_done:\n");
    // func end
    insert_line("add rsp, 64\n");
    insert_line("add rsp,20h\n");
    insert_line("ret\n");
    insert_line("input_bool endp\n");
}

/// <summary>
/// This function is used to tell the user that the input is invalid
/// </summary>
static void gen_invalid_input()
{
    insert_line("invalid_input proc\n");
    insert_line("sub rsp,28h\n");
    // print the msg
    insert_line("lea rdx, new_line\n");
    insert_line("call print_string\n");
    insert_line("lea rdx, invalid_input_str\n");
    insert_line("call print_string\n");
    // exit with error code
    insert_line("mov ecx, 1\n");
    insert_line("call ExitProcess\n");
    insert_line("invalid_input endp\n");

}

/// <summary>
/// This func is used to show buffer overflow error
/// </summary>
static void gen_buffer_overflow()
{
    insert_line("buffer_overflow proc\n");
    insert_line("sub rsp,20h\n");
    // print the msg
    insert_line("lea rdx, new_line\n");
    insert_line("call print_string\n");
    insert_line("lea rdx, buffer_overflow_str\n");
    insert_line("call print_string\n");
    // exit with error code
    insert_line("mov ecx, 1\n");
    insert_line("call ExitProcess\n");
    insert_line("buffer_overflow endp\n");
}