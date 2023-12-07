#pragma once

#include "set.h"
#include "ht.h"

void read_file(char*, char*);
void write_to_file(char*, char*, char*);
void clear_file(char* path);
void read_set_file(char* path, SET* my_set, char* struc);
void read_ht_file(char* path, HT* table, char* struc);
