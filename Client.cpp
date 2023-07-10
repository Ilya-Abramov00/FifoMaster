
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
        close(client_write_fd);
        close(client_count_write_fd);
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
        if (client_fd == -1) {{ std::cout << " ошибка: "; }}
        write(client_fd, data, strlen(data));


    }

    void ReadFifo(int client_fd, char *buffer) {
        if (client_fd == -1) {
            { std::cout << " ошибка: "; }
            memset(buffer, 0, sizeof(buffer));
            read(client_fd, buffer, MAXLINE);

        }
    }

    // std::string data0="ответное сообщение";
    // const char *data = data0.c_str();
    int8_t client_read_fd = -1;
    int8_t client_write_fd = -1;
    int8_t client_count_write_fd = -1;

    char client_buf[MAXLINE] = {0};
    char client_buf_count[1] = {1};
    std::string data_buffer = "";

};

int main() {


    sleep(0.1);
    std::cout << "клиент\n" << std::endl;

    ClientPipe a;
    a.PipeWrite();




    /*
    int client_read_fd = -1;
    int client_write_fd = -1;
    char client_buf[MAXLINE] = {0};

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
            read(client_read_fd, client_buf, MAXLINE);
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

    exit(EXIT_SUCCESS);*/
}
