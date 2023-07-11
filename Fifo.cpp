
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
const char *FIFO2 = "/home/ilya/Загрузки/Pipe/fifo2";

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR)
#define MAXLINE 10


class Fifo {
public:
    using MsgGetter = std::function<std::string()>;

    Fifo() {}

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

    void fifoWrite(std::string Fifo_write) {
        createFifo(Fifo_write.c_str());
        fifo_write_fd = openFifoWrite(Fifo_write.c_str());
        while (run) {
            writeFifo(fifo_write_fd, getmsg().c_str());
        }
    }
    void fifoRead(std::string Fifo_write,size_t N) {
        if (N>1024*64) {std::cout<<"ошибка";}
        fifo_read_fd = openFifoRead(Fifo_write.c_str());

        while (1) {

            readFifo(fifo_read_fd, read_buffer,N);
            std::cout << "прием от клиента  " << read_buffer << std::endl;
        }
    }

private:
    void createFifo(char const *FIFO) {
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

    void writeFifo(int fifo_fd, char const *data) {
        if (fifo_fd == -1) { std::cout << " ошибка: "; }
        write(fifo_fd, data, strlen(data));


    }

    void readFifo(int fifo_fd, char  *buffer, size_t N) {
        if (fifo_fd == -1) { std::cout << " ошибка: "; }
        memset(buffer, 0, N);
        read(fifo_fd, buffer, N);

    }

    int8_t fifo_write_fd = -1;
    int8_t fifo_read_fd = -1;
    std::string client_buf = "";
    bool run{false};
    MsgGetter getmsg;

    char *read_buffer = "";
    std::string data = "";

};

int main() {


    std::cout << "клиент" << std::endl << std::endl;

    Fifo a;
    auto getter = []() {
        std::string ret;

        while (std::cin >> ret) {
            return ret;
        }
    };
    a.setMsgGetter(getter);
    a.start();



    std::cout << "сервер завершил отправку" << std::endl;

    return 0;
}
