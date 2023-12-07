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

HT* create_ht(int);
void insert_ht(HT*, char* , char*);
void print_search(HT*, char*);
void print_ht(HT*);
void ht_del(HT*, char*);
void free_ht(HT*);
void save_ht(HT* table, char* path);
