#pragma once

#include "ht.h"

void write_to_file(char*, char*, char*);
void clear_file(char* path);
char* read_ht_file(char* path, HT* table, char* struc);
