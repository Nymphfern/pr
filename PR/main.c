#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#include <unistd.h>
#include <sys/types.h>

#define _SIZE_ 25

void call_dataBase(char* command, char* data, char* key)
{
	pid_t pid = -2;

	switch(pid = fork())
	{
	case -1:
		printf("ERROR: fork()");

		break;
	case 0:
		char* path = "/home/dmitriy/NSTU_proga/PR-1/dataBase.out\0";
		execl(path, command, data, key, NULL);
		//return;

		break;
	default:
		break;
	}
}

int main()
{
	char command[_SIZE_];
	printf("Enter the command: ");
	scanf("%s", command);

	char data[_SIZE_];
	char key[_SIZE_];

	//printf("Enter the data: ");
	//scanf("%s", data);

	//printf("Enter the key: ");
	//scanf("%s", key);

	if (strcmp(command, "SADD") == 0)
		{
			printf("Enter the data: ");
			scanf("%s", data);
			strcpy(key, "key");
		}
		else if (strcmp(command, "SPUSH") == 0)
		{
			printf("Enter the data: ");
			scanf("%s", data);
			strcpy(key, "key");
		}
		else if (strcmp(command, "QPUSH") == 0)
		{
			printf("Enter the data: ");
			scanf("%s", data);
			strcpy(key, "key");
		}
		else if (strcmp(command, "HSET") == 0)
		{
			printf("Enter the data: ");
			scanf("%s", data);

			printf("Enter the key: ");
			scanf("%s", key);

		}
		//------------REMOVE------------

		else if (strcmp(command, "SREM") == 0)
		{
			printf("Enter the key: ");
			scanf("%s", key);
			strcpy(data, "data");
		}
		else if (strcmp(command, "SPOP") == 0)
		{
			//printf("%s\n", pop_stack());
			strcpy(data, "data");
			strcpy(key, "key");
		}
		else if (strcmp(command, "QPOP") == 0)
		{
			//printf("%s\n", pop_queue());
			strcpy(data, "data");
			strcpy(key, "key");
		}
		else if (strcmp(command, "HDEL") == 0)
		{
			printf("Enter the key: ");
			scanf("%s", key);
			strcpy(data, "data");
		}

		//------------READ------------
		else if (strcmp(command, "SISMEMBER") == 0)
		{
			printf("Enter the key: ");
			scanf("%s", key);			
		}
		else if (strcmp(command, "HGET") == 0)
		{
			printf("Enter the key: ");
			scanf("%s", key);

		}
		else
		{
			printf("ERROR: incorrect command\n");
		}

	call_dataBase(command, data, key);
	
}
