
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

std::string FiFO1a = "/home/ilya/Загрузки/Pipe/fifo1";
std::string FiFO2a = "/home/ilya/Загрузки/Pipe/fifo2";
std::string FiFO3a = "/home/ilya/Загрузки/Pipe/fifo3";

auto FIFO1 = FiFO1a.c_str();
auto FIFO2 = FiFO2a.c_str();
auto FIFO3 = FiFO3a.c_str();

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR)
#define MAXLINE 15


class ClientPipe {
public:
    ClientPipe() {}


    void PipeWrite() {
        client_write_fd = OpenFifoWrite(FIFO1);
        client_count_write_fd = OpenFifoWrite(FIFO2);

        while (std::cin >> client_buf) {
            WriteFifo(client_count_write_fd, client_buf_count);

            WriteFifo(client_write_fd, client_buf);
            std::cout << "отправка серверу " << client_buf << std::endl;

            if (client_buf[0] == '0') { break; }
        }

      //  close(client_count_write_fd);
        //close(client_write_fd);
    }

/*~ClientPipe(){
    unlink(FIFO1);
    unlink(FIFO2);
    unlink(FIFO3);
    }*/

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

    void WriteFifo(int client_fd, char const *data) {
        if (client_fd == -1) { std::cout << " ошибка: "; }
        write(client_fd, data, strlen(data));


    }

    void ReadFifo(int client_fd, char *buffer) {
        if (client_fd == -1) { std::cout << " ошибка: "; }
        memset(buffer, 0, sizeof(buffer));
        read(client_fd, buffer, MAXLINE);

    }


    int8_t client_read_fd = -1;
    int8_t client_write_fd = -1;
    int8_t client_count_write_fd = -1;

    char client_buf[MAXLINE] = {0};
    char client_buf_count[1] = {1};
    std::string data_buffer = "";

};

int main() {


    std::cout << "клиент" << std::endl;

    ClientPipe a;
    a.PipeWrite();


    return 1;
}
