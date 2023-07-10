
#include <stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <cstring>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include <iostream>
#include <string>
#define FIFO1 "/home/ilya/Загрузки/Pipe/fifo1"
#define FIFO2 "/home/ilya/Загрузки/Pipe/fifo2"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR)
#define CLI_MAXLINE 1024



int main()
{

    std::cout<<"клиент\n";
sleep(5);
    std::cout<<"клиент\n";
    int client_read_fd = -1;
    int client_write_fd = -1;
    char client_buf[CLI_MAXLINE] = {0};

std::string data0;
    while(std::cin>>data0) {

        const char *data = data0.c_str();

        // Второй шаг, открываем соединение порта записи FIFO1 и порта чтения FIFO2
        client_write_fd = open(FIFO1, O_WRONLY, 0);
        if (-1 != client_write_fd) {
            std::cout << "Process  opening FIFO1 O_WRONLY";
            std::cout << getpid() << '\n';
        }
        client_read_fd = open(FIFO2, O_RDONLY, 0);
        if (-1 != client_read_fd) {
            std::cout << "Process  opening FIFO2 O_RDONLY";
            std::cout << getpid() << '\n';
        }


        // Третий шаг, клиент отправляет данные на сервер
        if (client_write_fd != -1) {
           
            write(client_write_fd, data, strlen(data) );
            std::cout << "client отправил message: ";
            std::cout << data << '\n';
        } else {
            exit(EXIT_FAILURE);
        }


        // Четвертый шаг, получение эхо-сообщения сервера
        if (client_read_fd != -1) {
            memset(client_buf, 0, sizeof(client_buf));
            read(client_read_fd, client_buf, CLI_MAXLINE);
            std::cout << "client получил message: ";
            std::cout << client_buf << '\n';
        } else {
            exit(EXIT_FAILURE);
        }


        // Шаг пятый: выход из процесса, закрытие ссылки
        close(client_read_fd);
        close(client_write_fd);
data0="";
        std::cout<< "\n";
    }
        unlink(FIFO1);
        unlink(FIFO2);
        std::cout << "Process finish\n";
        std::cout << getpid();

    exit(EXIT_SUCCESS);
}
