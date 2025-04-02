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
// registers for function calls based on MASM
static const char* symbRegisterNames[4] = { "rcx", "rdx", "r8", "r9" };

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
		sprintf(str, "%s", symbRegisterNames[s->offset]);
		break;
	default:
		printf("Invalid place of entry in symbol table");
		str[0] = '\0';
		break;
	}
	return str;
}

void createAsmFile() 
{
	FILE* out = fopen("C:\\Users\\itama\\Desktop\\output.asm", "w");
	if (!out) {
		perror("Error opening file");
		return 1;
	}
	fclose(out);
}

void insert_line(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char buffer[256];
	vsprintf(buffer, fmt, args);
	va_end(args);
	FILE* out = fopen("C:\\Users\\itama\\Desktop\\output.asm", "a");
	fprintf(out, "%s\n", buffer);
	fclose(out);
}