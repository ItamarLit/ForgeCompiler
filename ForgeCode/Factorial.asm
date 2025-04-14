extern ExitProcess: proc

extern GetStdHandle: proc

extern WriteConsoleA: proc

extern ReadConsoleA : proc

.data

_str0 db "Please enter a number",0
_str1 db "The factorial of the number you entered is",0
true_str db "true",0

false_str db "false",0

new_line db 13,10,0

number_buffer db 21 dup(0)

global_copy_buffer db 64 dup(0)

bytes_written dd 0

bytes_read dd 0

invalid_input_str db "The input type did not match, goodbye!",0

buffer_overflow_str db "Buffer overflow, max input 63 chars, goodbye!",0

.code

str_len proc

sub rsp,20h

xor r8d, r8d

len_loop:

cmp byte ptr [rdx + r8], 0

je len_done

inc r8d

jmp len_loop

len_done:

add rsp, 20h

ret

str_len endp

print_string proc

sub rsp,20h

mov rcx, -11

call GetStdHandle

mov rcx, rax
call str_len

lea r9, bytes_written

call WriteConsoleA

add rsp,20h

ret

print_string endp

print_bool proc

sub rsp,20h

cmp rdx, 0

je _print_false

lea rdx, true_str

jmp _print_done

_print_false:

lea rdx, false_str

_print_done:

call print_string

add rsp,20h

ret

print_bool endp

print_int proc

sub rsp,20h

mov rax, rdx

mov rbx, 10

cmp rax, 0

jge conv_start

neg rax

mov byte ptr [number_buffer], '-'

conv_start:

lea rdi, number_buffer + 20

mov byte ptr [rdi], 0

dec rdi

conv_loop:

xor rdx, rdx

div rbx

add dl, '0'

dec rdi

mov [rdi], dl

test rax, rax

jnz conv_loop

cmp byte ptr [number_buffer], '-'

jne skip_sign

dec rdi

mov al, '-'

mov [rdi], al

skip_sign:

lea rdx, [rdi]

call print_string

add rsp,20h

ret
print_int endp

input_string proc

sub rsp,20h

mov rcx, -10

call GetStdHandle

mov r8, rax

mov rcx, r8

mov rdx, rdi

mov r8d, 64

dec r8d

lea r9, bytes_read

xor rax, rax

call ReadConsoleA

xor rax, rax

mov eax, bytes_read

cmp rax, 63

jb skip_error

call buffer_overflow

skip_error:

xor rax, rax

mov eax, bytes_read

cmp rax, 0

je input_done

mov bl, byte ptr [rdi + rax - 2]

cmp bl, 13

jne no_carrige

mov byte ptr [rdi + rax - 2], 0

jmp input_done

no_carrige:

mov byte ptr [rdi + rax], 0

input_done:

add rsp,20h

ret

input_string endp

input_int proc

sub rsp,20h

sub rsp, 64

lea rdi, [rsp]

call input_string

xor rax, rax

xor rcx, rcx

mov r8b, 0

movzx rdx, byte ptr [rdi]

cmp rdx, 0

je invalid_input

cmp rdx, '-'

jne parse_loop

mov r8b, 1

inc rcx

movzx rdx, byte ptr [rdi + rcx]

cmp rdx, 0

je invalid_input

parse_loop:

movzx rdx, byte ptr [rdi + rcx]

cmp rdx, 0

je parse_done

cmp rdx, '0'

jl invalid_input

cmp rdx, '9'

jg invalid_input

sub rdx, '0'

imul rax, 10

add rax, rdx

inc rcx

jmp parse_loop

parse_done:

cmp r8b, 0

je parse_end

neg rax

parse_end:

add rsp, 64

add rsp,20h

ret

input_int endp

input_bool proc

sub rsp,20h

sub rsp, 64

lea rdi, [rsp]

call input_string

mov eax, 1

mov r8d, DWORD PTR [rdi]

cmp r8d, 'eurt'

jne is_false

movzx r9, byte ptr [rdi + 4]

cmp r9, 0

je bool_done

is_false:

xor eax, eax

mov r8, QWORD PTR [rdi]

cmp r8, QWORD PTR false_str

je bool_done

call invalid_input

bool_done:

add rsp, 64

add rsp,20h

ret

input_bool endp

invalid_input proc

sub rsp,28h

lea rdx, new_line

call print_string

lea rdx, invalid_input_str

call print_string

mov ecx, 1

call ExitProcess

invalid_input endp

buffer_overflow proc

sub rsp,20h

lea rdx, new_line

call print_string

lea rdx, buffer_overflow_str

call print_string

mov ecx, 1

call ExitProcess

buffer_overflow endp

copy_string proc

sub rsp,20h

push rcx

mov rcx, 8

copy_loop:

mov rax, [rsi]

mov [rdi], rax

add rsi, 8

add rdi, 8

loop copy_loop

pop rcx

add rsp,20h

ret

copy_string endp

Fac Proc

push rbp

mov rbp, rsp

sub rsp, 16

push rbx

push rdi

push rsi

push r12

push r13

push r14

push r15

mov [rbp + -8], rcx

mov rbx, [rbp + -8]

mov r10, 1

cmp rbx, r10

je _L2

mov rbx, 0

jmp _L3

_L2:

mov rbx, 1

_L3:

cmp rbx, 0

je _L0

mov rbx, 1

mov rax, rbx

jmp _Fac_ret

jmp _L1

_L0:

_L1:

mov rbx, [rbp + -8]

push rbx
sub rsp, 32

mov r10, [rbp + -8]

push r10
mov r11, 1

pop r10
sub r10, r11

mov rcx, r10

push rcx

push rdx

push r8

push r9

push r10

push r11

call Fac

pop r11

pop r10

pop r9

pop r8

pop rdx

pop rcx

mov r10, rax

add rsp, 32

pop rbx
imul rbx, r10

mov rax, rbx

jmp _Fac_ret

_Fac_ret:

add rsp, 16

pop r15

pop r14

pop r13

pop r12

pop rsi

pop rdi

pop rbx

pop rbp

ret

Fac Endp

Main Proc

sub rsp, 8
push rbp

mov rbp, rsp

sub rsp, 16

push rbx

push rdi

push rsi

push r12

push r13

push r14

push r15

mov rbx, 0

mov [rbp + -8], rbx

mov rbx, offset _str0

push rcx

push rdx

push r8

push r9

push r10

push r11

mov rdx, rbx

call print_string

lea rdx, new_line

call print_string

pop r11

pop r10

pop r9

pop r8

pop rdx

pop rcx

mov r10, [rbp + -8]

push rcx

push rdx

push r8

push r9

push r10

push r11

call input_int

mov [rbp + -8], rax

pop r11

pop r10

pop r9

pop r8

pop rdx

pop rcx

mov r11, offset _str1

push rcx

push rdx

push r8

push r9

push r10

push r11

mov rdx, r11

call print_string

lea rdx, new_line

call print_string

pop r11

pop r10

pop r9

pop r8

pop rdx

pop rcx

sub rsp, 32

mov r12, [rbp + -8]

mov rcx, r12

push rcx

push rdx

push r8

push r9

push r10

push r11

call Fac

pop r11

pop r10

pop r9

pop r8

pop rdx

pop rcx

mov r12, rax

add rsp, 32

push rcx

push rdx

push r8

push r9

push r10

push r11

mov rdx, r12

call print_int

lea rdx, new_line

call print_string

pop r11

pop r10

pop r9

pop r8

pop rdx

pop rcx

_Main_ret:

add rsp, 16

pop r15

pop r14

pop r13

pop r12

pop rsi

pop rdi

pop rbx

pop rbp

mov ecx, 0

call ExitProcess

Main Endp

END

