#pragma warning (disable:4996)
#include "ParseStack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// assignemnt functions for the different data types
void assignState(StackEntry* entry, StackData data)
{ 
	entry->data.state = data.state;
}

void assignSymbol(StackEntry* entry, StackData data)
{ 
	entry->data.symbol = strdup(data.symbol); 
}

void assignToken(StackEntry* entry, StackData data) 
{
	entry->data.token = data.token; 
}

AssignFunc assignFuncs[] = {
	assignState,  // STATE = 0
	assignToken,   // TOKEN = 1
	assignSymbol, // SYMBOL = 2

};

/// <summary>
/// Init func for the stack
/// </summary>
/// <returns></returns>
Stack* InitStack()
{
	Stack* sptr = (Stack*)malloc(sizeof(Stack));
	if (!sptr) {
		printf("Failed to allocate memory for the stack");
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
void PushStack(Stack* s, StackData data, StackDataType type)
{
	StackEntry** temp = realloc(s->items, sizeof(StackEntry*) * (s->top + 2));
	if (temp == NULL)
		return;
	else {
		s->items = temp;
		StackEntry* entry = (StackEntry*)malloc(sizeof(StackEntry));
		if (!entry) {
			printf("Failed to allocate memory for stack entry pointer");
			return;
		}
		// assign the data
		assignFuncs[type](entry, data);
		entry->type = type;
		s->items[++s->top] = entry;
	}
}

/// <summary>
/// This func checks if the stack is empty
/// </summary>
/// <param name="s"></param>
/// <returns></returns>
int IsStackEmpty(Stack* s)
{
	return s->top == -1;
}

/// <summary>
/// Pop func for the stack
/// </summary>
/// <param name="s"></param>
/// <returns>Returns a pointer to StackEntry from the top of the stack</returns>
StackEntry* PopStack(Stack* s)
{
	if (!IsStackEmpty(s)) {
		StackEntry* x = s->items[s->top--];
		if (x->type == SYMBOL) {
			free(x->data.symbol);
		}
		s->items = realloc(s->items, sizeof(StackEntry*) * (s->top + 1));
		return x;
	}
	return NULL;
}

/// <summary>
/// Top func for the stack
/// </summary>
/// <param name="s"></param>
/// <returns>Returns a pointer to StackEntry from the top of the stack</returns>
StackEntry* TopStack(Stack* s)
{
	if (IsStackEmpty(s))
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
void FreeStack(Stack* s) {
	for (int i = 0; i <= s->top; i++) {
		if (s->items[i]->type == SYMBOL) {
			free(s->items[i]->data.symbol);
		}
		free(s->items[i]);  
	}
	free(s->items);
	free(s);
}