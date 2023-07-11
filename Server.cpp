
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


#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR)
#define MAXLINE 5


const char* FIFO1 ="/home/ilya/Загрузки/Pipe/fifo1";
const char* FIFO2 ="/home/ilya/Загрузки/Pipe/fifo2";


class ServerPipe {
public:
    ServerPipe() {
        createPipe(FIFO1);

    }



    void ServerRead(size_t N) {
        if (N>1024*64) {std::cout<<"ошибка";}
        server_read_fd = openFifoRead(FIFO1);

        while (1) {

            readFifo(server_read_fd, server_buf,N);
            std::cout << "прием от клиента  " << server_buf << std::endl;
        }
    }

//    ~ServerPipe() {
//        exit(EXIT_SUCCESS);
//    }

private:

    void createPipe(char const *FIFO) {
        if ((mkfifo(FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
            std::cout << "can't creat ";
            std::cout << FIFO << '\n';
        }
    }

    int openFifoRead(char const *FIFO) {
        int fd = open(FIFO, O_RDONLY, 0);
        if (-1 == fd) {
            std::cout << "Все плохо Read\n";
        }
        return fd;
    }

    int openFifoWrite(char const *FIFO) {
        int fd = open(FIFO, O_WRONLY, 0);
        if (-1 == fd) {
            std::cout << "Все плохо  Write\n";
        }
        return fd;
    }

    void writeFifo(int server_fd, char const *data) {
        if (server_fd == -1) { std::cout << " ошибка: "; }
        write(server_fd, data, strlen(data));


    }

    void readFifo(int server_fd, char *buffer,size_t N) {
        if (server_fd == -1) { std::cout << " ошибка: "; }
        memset(buffer, 0, N);
        read(server_fd, buffer, N);

    }

    int8_t server_read_fd = -1;
    char server_buf[MAXLINE] = {0};
    std::string data_buffer = "";

};

int main() {

    std::cout << "сервер" << std::endl<< std::endl;

    ServerPipe a;
    a.ServerRead(20);

    std::cout << "сервер окончил прием" << std::endl;
    return 1;
}