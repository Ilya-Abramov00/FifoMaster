
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
#define MAXLINE 15


const char* FIFO1 ="/home/ilya/Загрузки/Pipe/fifo1";
const char* FIFO2 ="/home/ilya/Загрузки/Pipe/fifo2";
const char* FIFO3 = "/home/ilya/Загрузки/Pipe/fifo3";

class ServerPipe {
public:
    ServerPipe() {
        CreatePipe(FIFO1);
        CreatePipe(FIFO2);
        CreatePipe(FIFO3);
    }



    void ServerRead() {
        server_read_fd = OpenFifoRead(FIFO1);
        server_count_read_fd = OpenFifoRead(FIFO2);
        int k=0,i = 0;

        while (1) {
            ReadFifo(server_count_read_fd, server_buf_count);

            k += server_buf_count[0];
            if ((i + 1) == k) {

                ReadFifo(server_read_fd, server_buf);
                std::cout << "прием от клиента  " << server_buf << std::endl;

                if (server_buf[0] == '1') {
                    break;
                }
            }
            server_buf_count[0]=0;
            i = k;
        }

    }

//    ~ServerPipe() {
//        exit(EXIT_SUCCESS);
//    }

private:

    void CreatePipe(char const *FIFO) {
        if ((mkfifo(FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
            std::cout << "can't creat ";
            std::cout << FIFO << '\n';
        }
    }

    int OpenFifoRead(char const *FIFO) {
        int fd = open(FIFO, O_RDONLY, 0);
        if (-1 == fd) {
            std::cout << "Все плохо Read\n";
        }
        return fd;
    }

    int OpenFifoWrite(char const *FIFO) {
        int fd = open(FIFO, O_WRONLY, 0);
        if (-1 == fd) {
            std::cout << "Все плохо  Write\n";
        }
        return fd;
    }

    void WriteFifo(int server_fd, char const *data) {
        if (server_fd == -1) { std::cout << " ошибка: "; }
        write(server_fd, data, strlen(data));


    }

    void ReadFifo(int server_fd, char *buffer) {
        if (server_fd == -1) { std::cout << " ошибка: "; }
        memset(buffer, 0, sizeof(buffer));
        read(server_fd, buffer, MAXLINE);

    }

    int8_t server_read_fd = -1;
    int8_t server_count_read_fd = -1;
    int8_t server_write_fd = -1;


    char server_buf[MAXLINE] = {0};
    char server_buf_count[1];
    std::string data_buffer = "";

};

int main() {

    std::cout << "сервер" << std::endl;

    ServerPipe a;
    a.ServerRead();


    return 1;
}