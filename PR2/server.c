#include "queue.h"
#include "stack.h"
#include "set.h"
#include "ht.h"
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

#define commad_line "enter the command\n"

#define data_line "Data\n"

#define key_line "Key\n"

#define done_line "Done! enter the command\n"

#define msg_error "ERROR: incorrect_command\n"

#define not_found "ERROR: not_exsist Enter the command\n"

#define _SIZE_ 15

#define _BUFF_SIZE_ 20 * 1024

#define CLEAR_BUFF memset(buff, '\0', sizeof(buff));

void dostuff(int sock, int shm, HT* table, SET* my_set);

void error(const char* msg) { 

    perror(msg);

    exit(EXIT_FAILURE);
}
int nclients = 0;
void printusers()
{
    if (nclients) {
        printf("%d user on-line\n", nclients);
    }
    else {
        printf("No User on-line\n");
    }
}
int main(int argc, char* argv[]) {
    char buff[1024]; // ����� ��� �������� ������
    int sockfd, newsockfd; // ���������� ��� �������� ������������ �������
    int portno; // ���������� ��� �������� ������ �����
    int pid; // ���������� ��� �������� �������������� ��������
    socklen_t clilen; // ���������� ��� �������� ������� ������ �������
    struct sockaddr_in serv_addr, cli_addr; // ��������� ��� �������� ������� ������� � �������

    if (argc < 2) { // ���������, ������� �� ����� ����� ��� �������� ��������� ������
        fprintf(stderr, "ERROR: no port provided\n"); // ������� ��������� �� ������ ���������� ������ �����
        exit(EXIT_FAILURE); // ��������� ���������
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // ������� TCP-�����
    if (sockfd < 0) error("ERROR: opening socket"); // ��������� ���������� �������� ������

    bzero((char*)&serv_addr, sizeof(serv_addr)); // �������� ��������� ������ �������
    portno = atoi(argv[1]); // ����������� ����� ����� �� ������ � ����� �����
    serv_addr.sin_family = AF_INET; // ������������� ��������� ������� �� IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY; // ������������� IP-����� ������� �� ����� ���������
    serv_addr.sin_port = htons(portno); // ������������� ���� �������

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) { // ��������� ����� � ������� �������
        error("ERROR: on binding"); // ��������� ���������� ���������� ������ � ������� �������
    }

    listen(sockfd, 5); // ������������� ����� � ����� ������������� ���������� � ������������ �������� �������� 5

    clilen = sizeof(cli_addr); // �������� ������ ������ �������

    int shm = shm_open("/shm", O_CREAT | O_RDWR, 0660); // ��������� ����������� ������ ��� ������ ������� ����� ����������
    if (shm == -1) {
        error("Shared memory open"); // ��������� ���������� �������� ����������� ������
    }

    if (ftruncate(shm, sizeof(int)) == -1) { 
        error("Shared memory truncate"); // ��������� ���������� ��������� ������� ����������� ������
    }

    char* addr = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0); // ������� ����������� ����������� ������ � �������� ������������ ��������
    memcpy(addr, &nclients, sizeof(nclients)); // �������� �������� ���������� nclients � ����������� ������

    HT* table = create_table(_SIZE_); // ������� ���-������� ��������� �������
    SET* my_set = create_set(_SIZE_); // ������� ��������� ��������� �������

    while (1) {

        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);

        if (newsockfd < 0) error("ERROR: on accept");

        memcpy(&nclients, addr, sizeof(nclients));

        nclients++;

        memcpy(addr, &nclients, sizeof(nclients));

        struct hostent* hst;

        hst = gethostbyaddr((char*)&cli_addr.sin_addr, 4, AF_INET);

        printf("+%s [%s] new connect!\n",

            (hst) ? hst->h_name : "Unknown host",

            (char*)inet_ntoa(cli_addr.sin_addr));

        printusers();

        pid = fork();

        if (pid < 0) error("ERROR: on fork");

        if (pid == 0) {

            close(sockfd);

            dostuff(newsockfd, shm, table, my_set);

            exit(EXIT_SUCCESS);
        }
        else close(newsockfd);
    }
    close(sockfd);
    exit(EXIT_SUCCESS);
}
void dostuff(int sock, int shm, HT* table, SET* my_set)
{
    int bytes_recv;
    char buff[_BUFF_SIZE_];
    char* addr = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

    while (1)
    {
        write(sock, commad_line, sizeof(commad_line));
        bytes_recv = read(sock, &buff[0], sizeof(buff));
        if (bytes_recv < 0) error("ERROR reading from socket");
        if (!strcmp(buff, "end\n")) break;
        if (strcmp(buff, "SADD\n") == 0)
        {
            write(sock, data_line, sizeof(data_line));
            bytes_recv = read(sock, &buff[0], sizeof(buff));
            if (bytes_recv < 0) error("ERROR reading from socket");
            set_insert(my_set, buff, "");
            write(sock, done_line, sizeof(done_line));
        }
        else if (strcmp(buff, "SPUSH\n") == 0)
        {
            write(sock, data_line, sizeof(data_line));
            bytes_recv = read(sock, &buff[0], sizeof(buff));
            if (bytes_recv < 0) error("ERROR reading from socket");
            push_stack(buff);
            write(sock, done_line, sizeof(done_line));
        }
        else if (strcmp(buff, "QPUSH\n") == 0)
        {
            write(sock, data_line, sizeof(data_line));
            bytes_recv = read(sock, &buff[0], sizeof(buff));
            if (bytes_recv < 0) error("ERROR reading from socket");
            push_queue(buff);
            write(sock, done_line, sizeof(done_line));
        }
        else if (strcmp(buff, "HSET\n") == 0)
        {
            write(sock, data_line, sizeof(data_line));
            bytes_recv = read(sock, &buff[0], sizeof(buff));
            if (bytes_recv < 0) error("ERROR reading from socket");
            char* data = strdup(buff);
            memset(buff, '\0', sizeof(buff));

            write(sock, key_line, sizeof(data_line));
            bytes_recv = read(sock, &buff[0], sizeof(buff));
            if (bytes_recv < 0) error("ERROR reading from socket");
            ht_insert(table, buff, data);
            write(sock, done_line, sizeof(done_line));
        }

        else if (strcmp(buff, "SREM\n") == 0)
        {
            write(sock, data_line, sizeof(data_line));
            bytes_recv = read(sock, &buff[0], sizeof(buff));

            if (bytes_recv < 0) error("ERROR reading from socket");
            set_delete(table, buff);
        }
        else if (strcmp(buff, "SPOP\n") == 0)
        {
            char* msg_res = pop_stack();
            write(sock, msg_res, sizeof(msg_res));
        }
        else if (strcmp(buff, "QPOP\n") == 0)
        {
            char* msg_res = pop_queue();
            write(sock, msg_res, sizeof(msg_res));
        }
        else if (strcmp(buff, "HDEL\n") == 0)
        {
            write(sock, key_line, sizeof(data_line));
            bytes_recv = read(sock, &buff[0], sizeof(buff));
            if (bytes_recv < 0) error("ERROR reading from socket");
            ht_delete(table, buff);
        }

        else if (strcmp(buff, "SISMEMBER\n") == 0)
        {
            printf("Why?\n");
        }
        else if (strcmp(buff, "HGET\n") == 0)
        {
            CLEAR_BUFF
                write(sock, key_line, sizeof(data_line));
            bytes_recv = read(sock, &buff[0], sizeof(buff));
            if (bytes_recv < 0) error("ERROR reading from socket");
            printf("%s", buff);
            char* res = ht_search(table, buff);
            if (res == NULL)
            {
                write(sock, not_found, sizeof(not_found));
            }
            write(sock, res, sizeof(res));
        }
        else
        {
            write(sock, msg_error, sizeof(msg_error));
        }
        CLEAR_BUFF
    }
    memcpy(&nclients, addr, sizeof(nclients));
    nclients--; 
    memcpy(addr, &nclients, sizeof(nclients));
    close(sock);
    printf("-disconnect\n");
    printusers();
    return;
}

