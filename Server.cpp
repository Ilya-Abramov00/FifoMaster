
#include <stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <cstring>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<vector>

#include "iostream"


#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR)
#define MAXLINE 1024

std::string FiFO1a = "/home/ilya/Загрузки/Pipe/fifo1";
std::string FiFO2a = "/home/ilya/Загрузки/Pipe/fifo2";
std::string FiFO3a = "/home/ilya/Загрузки/Pipe/fifo3";

auto FIFO1 = FiFO1a.c_str();
auto FIFO2 = FiFO2a.c_str();
auto FIFO3 = FiFO3a.c_str();

class ServerPipe {
public:
    ServerPipe() {
        CreatePipe(FIFO1);
        CreatePipe(FIFO2);
        CreatePipe(FIFO3);
    }

    void StartPipe() {
        server_read_fd = OpenFifo(FIFO1);
        server_write_fd = OpenFifo(FIFO2);
        server_count_read_fd = OpenFifo(FIFO3);

        ReadFifo(server_count_read_fd, server_buf_count);
        int i=0;
        while(i!=4)
        {
            sleep(0.0005);
            ReadFifo(server_read_fd, server_buf);
            data+=server_buf;
            i++;
        }
        std::cout<<data;
    }


private:
    void CreatePipe(char const *FIFO) {
        if ((mkfifo(FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
            std::cout << "can't creat ";
            std::cout << FIFO << '\n';
        }
    }

    int OpenFifo(char const *FIFO) {
        int fd = open(FIFO, O_WRONLY, 0);
        if (-1 != fd) {
            std::cout << "Process  opening FIFO2 O_WDONLY\n";
        }
        return fd;
    }

    void WriteFifo(int server_fd, char const *data) {
        if (server_fd != -1) {
            write(server_fd, data, strlen(data));
            std::cout << "server отправил message: ";
            std::cout << data << '\n';
        } else {
            std::cout << " ошибка отправки: ";
        }
    }

void ReadFifo(int server_fd, char *buffer) {
    if (server_fd != -1) {
        memset(buffer, 0, sizeof(buffer));
        read(server_fd, buffer, MAXLINE);
        std::cout <<"server получил message: \n";
        std::cout <<  buffer<<"\n";
    } else {
        exit(EXIT_FAILURE);
    }
}
    // std::string data0="ответное сообщение";
    // const char *data = data0.c_str();
    int server_read_fd = -1;
    int server_write_fd = -1;
    int server_count_read_fd = -1;

    char  server_buf[MAXLINE];
    char  server_buf_count[MAXLINE];
    std::string data;

};

int main() {
std::cout<<"сервер\n";
sleep (5);
    std::cout<<"сервер\n";
    ServerPipe a;
    a.StartPipe();


    return 1;
}