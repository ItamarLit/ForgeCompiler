#pragma warning (disable:4996)
#include "ParseStack.h"
#include "ErrorHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// assignemnt functions for the different data types
void assign_state(StackEntry* entry, StackData data)
{ 
	entry->data.state = data.state;
}

void assign_node(StackEntry* entry, StackData data)
{ 
	entry->data.node = data.node; 
}

AssignFunc assignFuncs[] = {
	assign_state,  // STATE = 0
	assign_node,   // NODE = 1
};

/// <summary>
/// Init func for the stack
/// </summary>
/// <returns></returns>
Stack* init_stack()
{
	Stack* sptr = (Stack*)malloc(sizeof(Stack));
	if (!sptr) {
		output_error(GENERAL, "Failed to allocate memory for the stack\n");
		return NULL;
	}
	sptr->top = -1;
	sptr->items = NULL;
	return sptr;
}

/// <summary>
/// Push func for the stack
/// </summary>
/// <param name="s"></param>
/// <param name="data"></param>
/// <param name="type"></param>
void push(Stack* s, StackData data, StackDataType type)
{
	StackEntry** temp = (StackEntry**)realloc(s->items, sizeof(StackEntry*) * (s->top + 2));
	if (!temp)
	{
		output_error(GENERAL, "Failed to reallocate memory for stack\n");
		return;
	}
	s->items = temp;
	StackEntry* entry = (StackEntry*)malloc(sizeof(StackEntry));
	if (!entry) {
		output_error(GENERAL, "Failed to allocate memory for stack entry pointer\n");
		return;
	}
	// assign the data
	assignFuncs[type](entry, data);
	entry->type = type;
	s->items[++s->top] = entry;
	
}

/// <summary>
/// This func checks if the stack is empty
/// </summary>
/// <param name="s"></param>
/// <returns></returns>
int is_stack_empty(Stack* s)
{
	return s->top == -1;
}

/// <summary>
/// Pop func for the stack
/// </summary>
/// <param name="s"></param>
/// <returns>Returns a pointer to StackEntry from the top of the stack</returns>
StackEntry* pop(Stack* s)
{
	if (!is_stack_empty(s)) {
		StackEntry* x = s->items[s->top--];
		StackEntry** temp = (StackEntry**)realloc(s->items, sizeof(StackEntry*) * (s->top + 1));
		if (!temp) 
		{
			output_error(GENERAL, "Failed to reallocate memory for stack\n");
			return;
		}
		s->items = temp;
		return x;
	}
	return NULL;
}

/// <summary>
/// Top func for the stack
/// </summary>
/// <param name="s"></param>
/// <returns>Returns a pointer to StackEntry from the top of the stack</returns>
StackEntry* top(Stack* s)
{
	if (is_stack_empty(s))
	{
		printf("Stack empty \n");
		return NULL;
	}
	return (s->items[s->top]);
}

/// <summary>
/// Free func for the stack
/// </summary>
/// <param name="s"></param>
void free_stack(Stack* s) {
	for (int i = 0; i <= s->top; i++) {
		if (s->items[i]->type == NODE) {
			free_AST_node(s->items[i]->data.node);
		}
		free(s->items[i]);  
	}
	free(s->items);
	free(s);
}