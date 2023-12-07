#include <stdio.h>
#include <string.h>
#include "queue.h"
#include "stack.h"
#include "set.h"
#include "ht.h"
#include "myfile.c"

#define _PATH_ "data.txt"
#define SIZE_ 15

void writer(HT* table, SET* my_set)
{
	clear_file(_PATH_);

	char* data = pop_queue();
	while(strcmp(data, "Is_empty.\n"))
	{
		write_to_file(_PATH_, "Queue", data);
		data = pop_queue();
	}

	data = pop_stack();
	while(strcmp(data, "Is_empty.\n"))
	{
		write_to_file(_PATH_, "Stack", data);
		data = pop_stack();
	}	
	
	save_set(my_set, _PATH_);
	save_table(table, _PATH_);

}

int main(int argc, char* argv[])
{	
	char* command = strdup(argv[0]);
	char* data = strdup(argv[1]);
	char* key = strdup(argv[2]);

	HT* table = create_table(SIZE_);
	SET* my_set = create_set(SIZE_);

	read_file(_PATH_, "Queue");
	read_file(_PATH_, "Stack");
	read_set_file(_PATH_, my_set, "Set");
	read_ht_file(_PATH_, table, "HT");

		if (strcmp(command, "SADD") == 0)
		{
			printf("\nData: %s\n", data);

			set_insert(my_set, data, "");
		}
		
		else if (strcmp(command, "SREM") == 0)
		{
			printf("\nKey: %s\n", key);

			set_delete(table, key);
			
			key = strcat(key, "\n");
		}
		
		else if (strcmp(command, "SISMEMBER") == 0)
		{
			char* tmp;

			key = strcat(key, "\n");

			if((tmp = set_search(my_set, key)) == NULL)
			{
				printf("---NULL---\n");	
			}
			else
			{
				printf("Exist\n");
			}
			
		}
		
		else if (strcmp(command, "SPUSH") == 0)
		{
			printf("\nData: %s\n", data);

			push_stack(data);

		}
		
		else if (strcmp(command, "SPOP") == 0)
		{
			printf("%s\n", pop_stack());
		}
		
		else if (strcmp(command, "QPUSH") == 0)
		{
			printf("\nData: %s\n", data);

			push_queue(data);

		}
		
		else if (strcmp(command, "QPOP") == 0)
		{
			printf("%s\n", pop_queue());
		}
		
		else if (strcmp(command, "HSET") == 0)
		{
			printf("\nKey: %s data: %s\n", key, data);

			ht_insert(table, key, data);
		}

		else if (strcmp(command, "HDEL") == 0)
		{
			printf("\nKey: %s\n", key);

			ht_delete(table, key);
		}

		else if (strcmp(command, "HGET") == 0)
		{
			printf("\nKey: %s\n", key);

			print_search(table, key);
		}
		
		else
		{
			printf("Unknown command\n");
		}

		writer(table, my_set);
}
