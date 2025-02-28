#ifndef PARSE_STACK_H 
#define PARSE_STACK_H

typedef int stack_item;

typedef struct {
	int top;
	stack_item* data;
} stack;

void stack_init(stack*);
int  stack_empty(stack*);
int  stack_full(stack*);
stack_item stack_pop(stack*);
void stack_push(stack*, stack_item);
stack_item stack_top(stack*);

#endif