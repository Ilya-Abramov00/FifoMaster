
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
#include <functional>

const char *FIFO1 = "/home/ilya/Загрузки/Pipe/fifo1";
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR)
#define MAXLINE 10


class ClientPipe {
public:
    using MsgGetter = std::function<std::string()>;

    ClientPipe() {}

    void start() {
        if (!getmsg) {
            throw std::runtime_error("callback for msg getting not set");
        }
        run = true;
    }

    void stop() {
        run = false;
    }

    void setMsgGetter(MsgGetter msgGetter) {
        getmsg = std::move(msgGetter);
    }

    void pipeWrite() {
        client_write_fd = openFifoWrite(FIFO1);
        while (run) {
            writeFifo(client_write_fd, getmsg().c_str());
        }
    }

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

    void writeFifo(int client_fd, char const *data) {
        if (client_fd == -1) { std::cout << " ошибка: "; }
        write(client_fd, data, strlen(data));


    }

    void readFifo(int client_fd, char *buffer) {
        if (client_fd == -1) { std::cout << " ошибка: "; }
        memset(buffer, 0, sizeof(buffer));
        read(client_fd, buffer, sizeof(buffer));

    }

    int8_t client_write_fd = -1;
    std::string client_buf = "";
    bool run{false};
    MsgGetter getmsg;
};

int main() {


    std::cout << "клиент" << std::endl<< std::endl;

    ClientPipe a;
    auto getter = []() {
        std::string ret;

        while (std::cin >> ret) {
            return ret;
        }
    };
    a.setMsgGetter(getter);
    a.start();

    a.pipeWrite();

    std::cout << "сервер завершил отправку" << std::endl;

    return 0;
}
