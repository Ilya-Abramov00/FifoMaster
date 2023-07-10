
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

    int k = 0;
    int i = 0;

    void StartPipe() {
        server_read_fd = OpenFifoRead(FIFO2);
//        server_write_fd = OpenFifoWrite(FIFO1);
        server_count_read_fd = OpenFifoRead(FIFO3);

        while (1) {
            ReadFifo(server_count_read_fd, server_buf_count);

            k += server_buf_count[0];
            if ((i + 1) == k) {

                ReadFifo(server_read_fd, server_buf);
                std::cout << "прием от клиента  " << server_buf << std::endl;
            }
            i = k;
        }
    }

private:
    void CreatePipe(char const *FIFO) {
        if ((mkfifo(FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
            std::cout << "can't creat ";
            std::cout << FIFO << '\n';
        }
    }

    int OpenFifoRead(char const *FIFO) {
        int fd = open(FIFO, O_RDONLY, 0);
        if (-1 != fd) {
            std::cout << "Process opening Read\n";
        }
        return fd;
    }

    int OpenFifoWrite(char const *FIFO) {
        int fd = open(FIFO, O_WRONLY, 0);
        if (-1 != fd) {
            std::cout << "Process opening  Write\n";
        }
        return fd;
    }

    void WriteFifo(int server_fd, char const *data) {
        if (server_fd != -1) {
            write(server_fd, data, strlen(data));

        } else {
            std::cout << " ошибка отправки: ";
        }
    }

    void ReadFifo(int server_fd, char *buffer) {
        if (server_fd != -1) {
            memset(buffer, 0, sizeof(buffer));
            read(server_fd, buffer, MAXLINE);

        } else {
            exit(EXIT_FAILURE);
        }
    }

    // std::string data0="ответное сообщение";
    // const char *data = data0.c_str();
    int server_read_fd = -1;
    int server_write_fd = -1;
    int server_count_read_fd = -1;

    char server_buf[MAXLINE];
    char server_buf_count[1];
    std::string data;

};

int main() {
    sleep(0.5);
    std::cout << "сервер\n" << std::endl;


    ServerPipe a;
    a.StartPipe();


    return 1;
}