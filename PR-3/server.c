#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ht.c"
#include "myfile.c"

#define _PATH_ "data.txt"
#define CONST 15
#define SIIZE 20 * 1024
#define msg_success "SUCCESS"

#define msg_error "ERROR:_INCORRECT_MESSAGE"
#define not_found "ERROR:_NOT_FOUND"

#define CLEAR_BUFF memset(buff, '\0', sizeof(buff));

void dostuff (int sock, int shm);

void error(const char *msg) { //closed if error
    perror(msg);
    exit(EXIT_FAILURE);
}
int countcli = 0;
void printusers() 
{
    if (countcli) {
        printf("%d user online\n", countcli);
    }
    else {
        printf("No user online\n");
    }
}

char* _decapitation(const char* input, char* word)
{
    char* output = strtok(input, "#");

    if (strcmp(output, word) == 0)
    {
        return strtok(NULL, "#");
    }
    else
    {
        return NULL;
    }
}

char* selection(char* input, int start, int len)
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

int main(int argc, char *argv[]) {
    char buff[1024]; 
    int sockfd, newsockfd; 
    int numberport; 
    int potokid; 
    socklen_t clien; 
    struct sockaddr_in serv_addr, cli_addr; 
    
    if (argc < 2) {
        fprintf(stderr, "No port provided\n");
        exit(EXIT_FAILURE);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("Opening socket");
    
    bzero((char*) &serv_addr, sizeof(serv_addr));
    numberport = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_addr.sin_port = htons(numberport);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("On binding");
    }
    
    listen(sockfd, 5);
    clien = sizeof(cli_addr);
    
    int shm = shm_open("/shm", O_CREAT | O_RDWR, 0660);
    if (shm == -1) 
    {
    	error("Shared memory open");
    }
    
    if (ftruncate(shm, sizeof(int)) == -1) 
    {
    	error("Shared memory truncate");
    }
    
    char* addr = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    memcpy(addr, &countcli, sizeof(countcli));

    HT* table = create_table(CONST);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clien);
        if (newsockfd < 0) error("ERROR: on accept");
        memcpy(&countcli, addr, sizeof(countcli));
        countcli++;
        memcpy(addr, &countcli, sizeof(countcli));

        struct hostent *hst;
        hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
        printf("+%s [%s] new connect\n",
        (hst) ? hst->h_name : "Unknown host",
        (char*)inet_ntoa(cli_addr.sin_addr));
        printusers();

        potokid = fork();
        if (potokid < 0) error("On fork");
        if (potokid == 0) {
            close(sockfd);
            dostuff(newsockfd, shm);
            exit(EXIT_SUCCESS);
        }
        else close(newsockfd);
    }
    close(sockfd);
    exit(EXIT_SUCCESS);
}


void dostuff (int sock, int shm)
{

    HT* table = create_table(CONST);
    int bytes_recv; 
    
    char buff[SIIZE];
    
    char* addr = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

    while (1) 
    {        
        bytes_recv = read(sock, &buff[0], sizeof(buff));
        char* tmp = strdup(buff);
        if (bytes_recv < 0) error("ERROR reading from socket");
        if (!strcmp(buff, "end")) break;

        if(strstr(buff, "PUSH") != 0)
        {
            char* key = _decapitation(buff, "PUSH");
            char* data = selection(tmp, 14, strlen(tmp));
            ht_insert(table, key, data);
            write_to_file(_PATH_, key, data);
            write(sock, msg_success, sizeof(msg_success));
            CLEAR_BUFF
        }
        else if(strstr(buff, "GET") != 0)
        {
            char* key = _decapitation(buff, "GET");
            
            char* my_msg = read_ht_file(_PATH_, table, key);        

            int len = strlen(my_msg);

            if(strcmp(my_msg, "NOT_FOUND"))
            {
                len--;
            }

            CLEAR_BUFF
            write(sock, my_msg, sizeof(char)*len);
        }
        else
        {            
            write(sock, msg_error, sizeof(msg_error));
        }

        CLEAR_BUFF

    }
    memcpy(&countcli, addr, sizeof(countcli));
    countcli--;
    memcpy(addr, &countcli, sizeof(countcli));
    close(sock);
    printf("-disconnect\n");
    
    printusers();
    return;
}
