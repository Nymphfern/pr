#include "queue.h"
#include "stack.h"
#include "set.h"
#include "ht.h"
#include "myfile.h"
#include <stdio.h>
#include <string.h>

char* decap(char* input, char* struc)
{
    char* output = strtok(input, ":");

    if (strcmp(output, struc) == 0)
    {
        return strtok(NULL, ":");
    }
    else
    {
        return NULL;
    }
}

void read_file(char* path, char* struc)
{
	FILE* mf = fopen(path, "r");
	

    if (!mf)
    {
        printf("File does not exist\n");
        return;
    }

    char mstr[100] = "Start";

    char* estr = "Start";

    while (estr != NULL)
    {
        estr = fgets(mstr, sizeof(mstr), mf);

        char* tmp = decap(mstr, struc);

        if (tmp != NULL)
        {

            if (!strcmp(struc, "Queue"))
            {
                push_queue(tmp);                
            }

            else if (!strcmp(struc, "Stack"))
            {
                push_stack(tmp);
            }
        }
    }

    fclose(mf);
}

void read_ht_file(char* path, HT* table, char* struc)
{
    FILE* mf = fopen(path, "r");
    

    if (!mf)
    {
        printf("File does not exist\n");
        return;
    }

    char mstr[100] = "Start";

    char* estr = "Start";

    while (estr != NULL)
    {
        estr = fgets(mstr, sizeof(mstr), mf);

        char* tmp = decap(mstr, struc);

        if (tmp != NULL)
        {

            if (!strcmp(struc, "HT"))
            {
                ht_insert(table, tmp, tmp);
            }
        }
    }

    fclose(mf);    
}
void write_to_file(char* path, char* struc, char* data)
{
    FILE* mf = fopen(path, "a");

    if (!mf)
    {
        printf("Cant open the file\n");
        
        return;
    }

    char* result = strdup(struc);
    
    char* tmp = strdup(data);

    fputs(result, mf);
    
    fputs(":", mf);
    
    fputs(tmp, mf);
    
    fputs("\n", mf);
    
    fclose(mf);
}

void read_set_file(char* path, SET* my_set, char* struc)
{
    FILE* mf = fopen(path, "r");
    

    if (!mf)
    {
        printf("File does not exist\n");
        return;
    }

    char mstr[100] = "Start";

    char* estr = "Start";

    while (estr != NULL)
    {
        estr = fgets(mstr, sizeof(mstr), mf);

        char* tmp = decap(mstr, struc);

        if (tmp != NULL)
        {
            if (!strcmp(struc, "Set"))
            {
                set_insert(my_set, tmp, "");
            }
        }
    }

    fclose(mf);    
}


void clear_file(char* path)
{
    FILE* mf = fopen(path, "w");

    if (!mf)
    {
        printf("Cant open the file\n");
        return;
    }

    fputs("", mf);

}
