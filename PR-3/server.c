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

#define _PATH_ "data.txt" // Путь к файлу данных
#define CONST 15 // Константа со значением 15
#define SIIZE 20 * 1024 // Размер сегмента памяти

#define cot_okay "OKAY" // Сообщение об успешном выполнении

#define cot_error "ERROR:_INCORRECT_MESSAGE" // Сообщение об ошибке: некорректное сообщение
#define not_found "ERROR:_NOT_FOUND" // Сообщение об ошибке: не найдено

#define CLEAR_BUFF memset(buff, '\0', sizeof(buff)); // Очистка буфера

void dostuff (int sock, int shm); // Функция для выполнения действий

void error(const char *cot) { // Функция для вывода ошибки и завершения программы
    perror(cot);
    exit(EXIT_FAILURE);
}
int countcli = 0;
// Функция для печати количества пользователей онлайн
void printusers() 
{
    if (countcli) {
        printf("%d user online\n", countcli);
    }
    else {
        printf("No user online\n");
    }
}
// Функция для отделения слова от строки
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
// Функция для выбора подстроки из заданной строки
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
    // Проверка наличия порта
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
    // Создание разделяемой памяти
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
    // Создание хэш-таблицы
    HT* table = create_table(CONST);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clien); // Принятие входящего соединения

        if (newsockfd < 0) error("ERROR: on accept"); // Вывод сообщения об ошибке, если принятие соединения не удалось

        memcpy(&countcli, addr, sizeof(countcli)); // Копирование значения countcli из памяти
        countcli++; // Увеличение значения countcli
        memcpy(addr, &countcli, sizeof(countcli)); // Копирование значения countcli в память

        struct hostent *hst; // Структура информации об хосте
        hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET); // Получение информации об хосте по его IP-адресу
        printf("+%s [%s] new connect\n",
        (hst) ? hst->h_name : "Unknown host", // Вывод имени хоста, если оно доступно, или "Unknown host", если имя неизвестно
        (char*)inet_ntoa(cli_addr.sin_addr)); // Вывод IP-адреса клиента
        printusers(); // Вывод информации о количестве пользователей онлайн

        potokid = fork(); // Создание нового процесса
        if (potokid < 0) error("On fork"); // Вывод сообщения об ошибке, если создание процесса не удалось
        if (potokid == 0) { // Если это дочерний процесс
            close(sockfd); // Закрытие дескриптора сокета в дочернем процессе
            dostuff(newsockfd, shm); // Выполнение действий в дочернем процессе
            exit(EXIT_OKAY); // Завершение дочернего процесса
        }
        else close(newsockfd); // Закрытие дескриптора сокета в родительском процессе
    }
    close(sockfd); // Закрытие дескриптора сокета
    exit(EXIT_OKAY); // Завершение программы
}


void dostuff (int sock, int shm)
{
    // Создание хэш-таблицы
    HT* table = create_table(CONST);
    int bytes_recv; 
    
    char buff[SIIZE];
    
    // Получение адреса разделяемой памяти
    char* addr = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

    while (1) 
    {        
        // Чтение данных из сокета
        bytes_recv = read(sock, &buff[0], sizeof(buff));
        char* tmp = strdup(buff);
        if (bytes_recv < 0) error("ERROR reading from socket");
        if (!strcmp(buff, "end")) break;

        // Обработка команды PUSH
        if(strstr(buff, "PUSH") != 0)
        {
            char* key = _decapitation(buff, "PUSH");
            char* data = selection(tmp, 14, strlen(tmp));
            ht_insert(table, key, data);
            write_to_file(_PATH_, key, data);
            write(sock, cot_okay, sizeof(cot_okay));
            CLEAR_BUFF
        }
        // Обработка команды GET
        else if(strstr(buff, "GET") != 0)
        {
            char* key = _decapitation(buff, "GET");
            
            char* my_cot = read_ht_file(_PATH_, table, key);        

            int len = strlen(my_cot);

            if(strcmp(my_cot, "NOT_FOUND"))
            {
                len--;
            }

            CLEAR_BUFF
            write(sock, my_cot, sizeof(char)*len);
        }
        // Обработка ошибочной команды
        else
        {            
            write(sock, cot_error, sizeof(cot_error));
        }

        CLEAR_BUFF

    }
    // Уменьшение счетчика пользователей онлайн
    memcpy(&countcli, addr, sizeof(countcli));
    countcli--;
    memcpy(addr, &countcli, sizeof(countcli));
    close(sock);
    printf("-disconnect\n");
    
    // Вывод количества пользователей онлайн
    printusers();
    return;
}
