#ifndef PARSE_STACK_H 
#define PARSE_STACK_H

#include "Token.h"

// union for the data type of the stack
typedef union 
{
	int state;
	char* symbol;
	Token* token;
} StackData;

// enum for the data type of the stack
typedef enum {
	STATE,
	TOKEN,
	SYMBOL,
} StackDataType;

// stack entry struct that holds the data and its type
typedef struct {
	StackData data;
	StackDataType type;
} StackEntry;

// stack struct, using a dynamic stack 
typedef struct {
	int top;
	StackEntry** items;
} Stack;

// create an assignFunc for the stack data 
typedef void (*AssignFunc)(StackEntry* entry, StackData data);

// different assignment funcs based on data type
void assignState(StackEntry* entry, StackData data); 
void assignSymbol(StackEntry* entry, StackData data); 
void assignToken(StackEntry* entry, StackData data);

// func table
extern AssignFunc assignFuncs[];

// func to init the stack 
Stack* InitStack();
// func to check if the stack is empty
int  IsStackEmpty(Stack* s);
// func that pops an item from the stack
StackEntry* PopStack(Stack* s);
// func that pushes an item onto the stack
void PushStack(Stack* s, StackData data, StackDataType type);
// func that returns the stack top
StackEntry* TopStack(Stack* s);
// func that frees the stack
void FreeStack(Stack* s);

#endif