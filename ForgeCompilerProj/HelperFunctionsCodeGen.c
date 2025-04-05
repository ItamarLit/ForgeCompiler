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
    gen_str_len();
    gen_print_string();
    gen_print_bool();
    gen_print_int();
    gen_input_string();
    gen_input_int();
    gen_input_bool();
    gen_invalid_input();
    gen_buffer_overflow();
    gen_copy_string();
}


static void gen_copy_string()
{
    insert_line("copy_string proc");
    insert_line("sub rsp,20h\n");
    // rdi = dest, rsi = src
    insert_line("push rcx");
    // 8 qwords
    insert_line("mov rcx, 8");  
    insert_line("copy_loop:");
    // load 8 bytes from src
    insert_line("mov rax, [rsi]");   
    // store 8 bytes to dest
    insert_line("mov [rdi], rax");   
    insert_line("add rsi, 8");       
    insert_line("add rdi, 8");      
    insert_line("loop copy_loop");
    insert_line("pop rcx");
    insert_line("add rsp,20h\n");
    insert_line("ret");
    insert_line("copy_string endp");
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
static void gen_input_int()
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
static void gen_input_bool()
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
static void gen_invalid_input()
{
    insert_line("invalid_input proc");
    insert_line("sub rsp,28h");
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
static void gen_buffer_overflow()
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