#include <stdio.h>
#include "ParseStack.h"


/*********************** Function bodies **************/

void stack_init(stack* sptr)
{
	sptr->top = -1;
}
	
void stack_push(stack* s, stack_item x)
{
	stack_item* temp = realloc(s->data, sizeof(stack_item) * (s->top + 2));
	if (temp == NULL)
		return;
	else {
		s->data = temp;
		s->data[++s->top] = x;
	}
}

int stack_empty(stack* sptr)    // if the stack empty return 1
{
	return sptr->top == -1;
}
int stack_full(stack* sptr)    // if the stack full return 1
{
	return 0;
}

stack_item stack_pop(stack* s)
{
	if (!stack_empty(s)) {
		stack_item x = s->data[s->top--];
		s->data = realloc(s->data, sizeof(stack_item) * (s->top + 1));
		return x;
	}
}

stack_item stack_top(stack* sptr)
{
	if (stack_empty(sptr))
	{
		printf("Stack empty \n");
		return (stack_item)NULL;
	}
	return (sptr->data[sptr->top]);
}
