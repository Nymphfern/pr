#pragma once

typedef struct ht_node {
    char* key;
    char* value;
} ht_node;

typedef struct HT {
    struct HT** items;
    struct List** overflow;
    int size;
    int count;
}HT;

typedef struct List {
    ht_node* ht_node;
    struct List* next;
    
} List;

HT* create_table(int);
void ht_insert(HT*, char* , char*);
void print_search(HT*, char*);
void print_table(HT*);
void ht_delete(HT*, char*);
void free_table(HT*);
void save_table(HT* table, char* path);
