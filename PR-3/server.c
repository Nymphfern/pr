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


#define msg_success "SUCCESS"
#define msg_error "ERROR:_INCORRECT_MESSAGE"
#define not_found "ERROR:_NOT_FOUND"

#define _PATH_ "data.txt"

#define _SIZE_ 15
#define _BUFF_SIZE_ 20 * 1024

#define CLEAR_BUFF memset(buff, '\0', sizeof(buff));

void dostuff (int sock, int shm);//, HT* table);

void error(const char *msg) {
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

// Функция отделения
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
    char buff[1024]; // Буфер для различных нужд
    int sockfd, newsockfd; // дескрипторы сокетов
    int portno; // номер порта
    int pid; // id номер потока
    socklen_t clilen; // размер адреса клиента типа socklen_t
    struct sockaddr_in serv_addr, cli_addr; // структура сокета сервера и клиента
    
    // ошибка в случае если мы не указали порт
    if (argc < 2) {
        fprintf(stderr, "No port provided\n");
        exit(EXIT_FAILURE);
    }
    
    // создание сокета
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("Opening socket");
    
    // связывание сокета с локальным адресом
    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // сервер принимает подключения на все IP-адреса
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("On binding");
    }
    
    // ожидание подключений, размер очереди - 5
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    
    // создание общей ячейки памяти для хранения количества активных пользователей
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

    HT* table = create_table(_SIZE_);

    // извлекаем сообщение из очереди (цикл извлечения запросов на подключение)
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) error("ERROR: on accept");
        memcpy(&countcli, addr, sizeof(countcli));
        countcli++;
        memcpy(addr, &countcli, sizeof(countcli));

        // вывод сведений о клиенте
        struct hostent *hst;
        hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
        printf("+%s [%s] new connect\n",
        (hst) ? hst->h_name : "Unknown host",
        (char*)inet_ntoa(cli_addr.sin_addr));
        printusers();

        pid = fork();
        if (pid < 0) error("On fork");
        if (pid == 0) {
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

    HT* table = create_table(_SIZE_);
    int bytes_recv; // размер принятого сообщения
    
    char buff[_BUFF_SIZE_];
    
    char* addr = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

    while (1) 
    {        
        // обработка первого параметра
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
    countcli--; // уменьшаем счетчик активных клиентов
    memcpy(addr, &countcli, sizeof(countcli));
    close(sock);
    printf("-disconnect\n");
    printusers();
    return;
}
