#ifndef PARSE_STACK_H 
#define PARSE_STACK_H

#include "Token.h"
#include "AST.h"

// union for the data type of the stack
typedef union 
{
	int state; 
	ASTNode* node;
} StackData;

// enum for the data type of the stack
typedef enum {
	STATE,
	NODE,
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
void assign_state(StackEntry* entry, StackData data); 
void assign_node(StackEntry* entry, StackData data); 

// func table
extern AssignFunc assignFuncs[];

// func to init the stack 
Stack* init_stack();
// func to check if the stack is empty
int is_stack_empty(Stack* s);
// func that pops an item from the stack
StackEntry* pop(Stack* s);
// func that pushes an item onto the stack
void push(Stack* s, StackData data, StackDataType type);
// func that returns the stack top
StackEntry* top(Stack* s);
// func that frees the stack
void free_stack(Stack* s);

#endif