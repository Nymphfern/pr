#include "ht.h"
#include "myfile.h"
#include <stdio.h>
#include <string.h>

char* decapitation(char* input, char* struc)
{
    char* output = strtok(input, "#");

    if (strcmp(output, struc) == 0)
    {
        return strtok(NULL, "#");
    }
    else
    {
        return NULL;
    }
}

char* _selection(char* input, int start, int len)
{
 int tmp = len - start;
 char* output = (char*)calloc(tmp, sizeof(char));
 output[tmp] = '\0';

 for (int i = start, j = 0; i < len; i++, j++)
 {
  output[j] = input[i];
 }

 return output;
}

char* read_ht_file(char* path, HT* table, char* struc)
{
    FILE* mf = fopen(path, "r");    

    if (!mf)
    {
        printf("ERROR: The file does not exist\n");
        return;
    }

    char mstr[100] = "Start";

    char* estr = "Start";

    while (estr != NULL)
    {
        estr = fgets(mstr, sizeof(mstr), mf);

        if((strstr(mstr, struc)) != NULL)
        {
            char* data = _selection(mstr, 9, strlen(mstr));
            ht_insert(table, struc, data);
            return _selection(mstr, 9, strlen(mstr));
        }
    }
    
    return "NOT_FOUND";

    fclose(mf);    
}

void write_to_file(char* path, char* struc, char* data)
{
    FILE* mf = fopen(path, "a");

    if (!mf)
    {
        printf("ERROR: Can't open the file\n");
        return;
    }

    char* result = strdup(struc);
    char* tmp = strdup(data);

    fputs(result, mf);
    fputs("#", mf);
    fputs(tmp, mf);
    fputs("\n", mf);
    fclose(mf);
}

void clear_file(char* path)
{
    FILE* mf = fopen(path, "w");

    if (!mf)
    {
        printf("ERROR: Can't open the file\n");
        return;
    }

    fputs("", mf);

}
