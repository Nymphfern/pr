#pragma once

#include <stdbool.h>

typedef struct queue_node
{
    char* data;
    struct queue_node* next;
} queue_node;

void push_queue(char*);
char* pop_queue();
bool empty();
