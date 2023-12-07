#pragma once

typedef struct node {
    char* key;
} node;

typedef struct SET {
    struct SET** items;
    int size;
    int count;
}SET;

SET* create_set(int size);
void set_insert(SET* table, char* key, char* value);
char* set_search(SET* table, char* key);
void search_print(SET*, char*);
void print_set(SET*);
void set_delete(SET*, char*);
void free_set(SET*);