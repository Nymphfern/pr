#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

stack_node* S_HEAD;
stack_node* S_TAIL;

void push_head(stack_node** list, char* data)
{
    stack_node* tmp = (stack_node*)malloc(sizeof(stack_node));
    tmp->data = strdup(data);
    tmp->next = *list;
    *list = tmp;
}

char* pop_head(stack_node** list)
{
    if (*list == NULL)
    { 
        return "List_is_empty.\n"; 
    }
    stack_node* tmp = *list;
    char* res = strdup(tmp->data);
    *list = tmp->next;
    free(tmp);
    return res;
}

void push_stack(char* data)
{
	push_head(&S_HEAD, data);
}

char* pop_stack()
{
	return pop_head(&S_HEAD);
}
