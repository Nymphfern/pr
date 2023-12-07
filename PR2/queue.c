#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

queue_node* START = NULL;
queue_node* END = NULL;

bool mpty()
{
    return START == NULL;
}

int count(queue_node* list_copy)
{
    int x = 0;
    for (; list_copy != NULL; list_copy = list_copy->next)
    {
        x++;
    }
    return x;
}

void push_beg(queue_node** list, char* data)
{
    queue_node* tmp = (queue_node*)malloc(sizeof(queue_node));
    //tmp->data = data;
    tmp->data = strdup(data);
    tmp->next = *list;
    *list = tmp;
}

char* pop_beg(queue_node** list)
{
    if (*list == NULL)
    {
        //printf("Element doesn't exist. List is empty.\n");
        return "List_empty.\n";
    }
    queue_node* tmp = *list;
    char* res = tmp->data;
    START = tmp->next;
    *list = tmp->next;
    free(tmp);
    return res;
}

void push_end(queue_node* list_copy, char* data)
{
    if (START == NULL)
    {
        push_beg(&START, data);
        END = START;
        return;
    }
    queue_node* tmp = (queue_node*)malloc(sizeof(queue_node));
    tmp->data = strdup(data);
    tmp->next = NULL;
    list_copy->next = tmp;
    END = tmp;
}

void push_queue(char* data)
{
    push_end(END, data);
}

char* pop_queue()
{
    return pop_beg(&START);
}
