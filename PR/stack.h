#pragma once

typedef struct stack_node
{
	char* data;
	struct stack_node* next;
} stack_node;

	void push_stack(char*);
	char* pop_stack();
