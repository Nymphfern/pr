#include "set.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define _SIZE 10 

unsigned long hash(char* str)
{
    unsigned long i = 0;
    for (int j = 0; str[j]; j++)
    {
        i = (i * 31) + i;
    }
    return i % _SIZE;
}

SET* create_set(int size)
{
    SET* table = (SET*)malloc(sizeof(SET));
    table->size = size;
    table->count = 0;
    table->items = (node**)calloc(table->size, sizeof(node*));
    for (int i = 0; i < table->size; i++)
    {
        table->items[i] = NULL;
    }

    return table;
}

node* create_node(char* key, char* value)
{
    node* item = (node*)malloc(sizeof(node));
    item->key = (char*)malloc(strlen(key) + 1);

    if (key != 0)
    {
        strcpy(item->key, key);
    }

    return item;
}


void free_node(node* item)
{
    free(item->key);
    free(item);
}

void free_set(SET* table)
{
    for (int i = 0; i < table->size; i++)
    {
        node* item = table->items[i];
        if (item != NULL)
        {
            free_node(item);
        }
    }

    free(table->items);
    free(table);
}

void set_insert(SET* table, char* key, char* value)
{
    if (table == NULL)
    {
        return;
    }
    // Create the item
    node* item = create_node(key, value);

    int index = hash_function(key);

    node* current_item = table->items[index];

    if (current_item == NULL)
    {
        if (table->count == table->size)
        {
            printf("HashTable is full\n");
            return;
        }

        table->items[index] = item;
        table->count++;
    }
    else
    {
        printf("Element already exists\n");
        return;
    }
}

char* set_search(SET* table, char* key)
{
    
    printf("\t%s\n", key);

    int index = hash(key);

    node* item = table->items[index];
    if (item == NULL)
    {
        return NULL;
    }
    else if (!strcmp(item->key, key))
    {
        return item->key;
    }

    printf("\t%s, %s", item->key, key);

    return NULL;
}

void search_print(SET* table, char* key)
{
    char* val;
    if ((val = set_search(table, key)) == NULL)
    {
        printf("Key:%s does not exist\n", key);
        return;
    }
    else
    {
        printf("Key:%s, Value:%s\n", key, val);
    }
}

void print_set(SET* table)
{
    printf("\nHash Table\n-------------------\n");
    for (int i = 0; i < table->size; i++)
    {
        if (table->items[i])
        {
            node* tmp = table->items[i];
            printf("Index:%d, Key:%s\n", i, tmp->key);
        }
    }
    printf("-------------------\n\n");
}

void save_set(SET* my_set, char* path)
{
    for (int i = 0; i < my_set->size; i++)
    {
        if (my_set->items[i])
        {
            node* tmp = my_set->items[i];
            write_to_file(path, "Set", tmp->key);
        }
    }
}

void set_delete(SET* table, char* key)
{

    int index = hash(key);

    if (table->items[index] == NULL)
    {
        return;
    }

    table->items[index] = NULL;
}
