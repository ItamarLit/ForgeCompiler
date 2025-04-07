#pragma warning (disable:4996)
#include "CodeGenUtils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// array that holds inuse registers
int registers[REG_COUNT] = { 0 };
// array that holds the amount of times a register was spilled
int reg_spilled[REG_COUNT] = { 0 };
// array that holds register names
const char* registerNames[REG_COUNT] = { "rbx", "r10", "r11", "r12", "r13", "r14", "r15"};
// win API param registers
char* paramRegisterNames[4] = { "rcx", "rdx", "r8", "r9" };
// global asm output path
char* asm_output_path = NULL;
int outputFlag = 0;

/// <summary>
/// This function will find a free register to use, if there isnt one it will push the first one and use it
/// </summary>
/// <returns>Returns an int that represents a free register</returns>
int scratch_alloc()
{
	for (int i = 0; i < REG_COUNT; i++) {
		if (registers[i] == 0) {
			registers[i] = 1;
			return i;
		}
	}
	// if no reg found, push the first reg to stack
	printf("push %s\n", scratch_name(0));
	registers[0] = 0;
	reg_spilled[0]++;
	// return the newly allocated register
	return scratch_alloc();
}

/// <summary>
/// This func will free a register based on its number, if the register was pushed the func pops it
/// </summary>
/// <param name="reg"></param>
void scratch_free(int reg)
{
	// if the reg was spilled then pop it
	if (reg_spilled[reg] > 0) {
		printf("pop %s\n", scratch_name(reg));
		reg_spilled[reg]--;
	}
	else {
		// free the register
		registers[reg] = 0;
	}
}

/// <summary>
/// This func will return the name of the reg based on an index
/// </summary>
/// <param name="reg"></param>
/// <returns>Returns a reg name</returns>
const char* scratch_name(int reg)
{
	return registerNames[reg];
}

/// <summary>
/// This func will create unique label names
/// </summary>
/// <returns>Returns a unique label name</returns>
const char* label_name()
{
	static int counter = 0;
	char* label = (char*)malloc(32);
	sprintf(label, "_L%d", counter++);
	return label;
}

/// <summary>
/// This func will generate code based on a symbols placement
/// </summary>
/// <param name="s"></param>
/// <returns>Returns a string that represents the entries code</returns>
const char* symbol_codegen(SymbolEntry* s)
{
	static char str[64];
	switch (s->place)
	{
	case IS_GLOBAL:
		sprintf(str, "%s", s->name);
		break;
	case IS_LOCAL:
		sprintf(str, "[rbp + %d]", s->offset);
		break;
	case IS_REG:
		sprintf(str, "%s", get_param_reg(s->offset));
		break;
	default:
		printf("Invalid place of entry in symbol table");
		str[0] = '\0';
		break;
	}
	return str;
}

/// <summary>
/// This func will create the asm file 
/// </summary>
void create_asm_file(char* path, int flag) 
{
	FILE* out = fopen(path, "w");
	if (!out) {
		perror("Error opening file");
		return 1;
	}
	asm_output_path = path;
	outputFlag = flag;
	fclose(out);
}

/// <summary>
/// This func will return a register name from the valid Windows Asm registers
/// </summary>
/// <param name="offset"></param>
/// <returns>Returns a name</returns>
char* get_param_reg(int offset) {
	return paramRegisterNames[offset];
}


void insert_line(const char* fmt, ...) {
	va_list args;
	// start from first arg after fmt
	va_start(args, fmt);
	char buffer[256];
	// fill the buffer with the formated string
	vsprintf(buffer, fmt, args);
	// clean up
	va_end(args);
	// write into the asm file
	FILE* out = fopen(asm_output_path, "a");
	fprintf(out, "%s\n", buffer);
	if(outputFlag) printf("%s\n", buffer);
	fclose(out);
}

// generate pushes for callee in windows conventions
void gen_callee_pushes()
{
	insert_line("push rbx\n");
	insert_line("push rdi\n");
	insert_line("push rsi\n");
	insert_line("push r12\n");
	insert_line("push r13\n");
	insert_line("push r14\n");
	insert_line("push r15\n");
}

// generate pops for callee in windows conventions
void gen_callee_pops()
{
	insert_line("pop r15\n");
	insert_line("pop r14\n");
	insert_line("pop r13\n");
	insert_line("pop r12\n");
	insert_line("pop rsi\n");
	insert_line("pop rdi\n");
	insert_line("pop rbx\n");
}

// generate pushes for caller in windows conventions
void gen_caller_pushes() 
{
	insert_line("push rcx\n");
	insert_line("push rdx\n");
	insert_line("push r8\n");
	insert_line("push r9\n");
	insert_line("push r10\n");
	insert_line("push r11\n");
}

// generate pops for caller in windows conventions
void gen_caller_pops() 
{
	insert_line("pop r11\n");
	insert_line("pop r10\n");
	insert_line("pop r9\n");
	insert_line("pop r8\n");
	insert_line("pop rdx\n");
	insert_line("pop rcx\n");
}