#ifndef COMPLEX_H
#define COMPLEX_H

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


#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR)
#define MAXLINE 64*1024


class Fifo {
public:
    using MsgGetter = std::function<std::string()>;

    Fifo() {}

    void setMsgGetter(MsgGetter msgGetter) {
        getmsg = std::move(msgGetter);
    }

    void start_write() {
        if (!getmsg) {
            throw std::runtime_error("callback for msg getting not set");
        }
        run_write = true;
    }

    void stop_write() {
        run_write = false;
    }

    void fifoWrite(std::string &Fifo_write) {
        createFifo(Fifo_write.c_str());
        int8_t fifo_write_fd = openFifoWrite(Fifo_write.c_str());
        while (run_write) {
            writeFifo(fifo_write_fd, getmsg().c_str());
        }
    }

    void start_read() {
        run_read = true;
    }

    void stop_read() {
        run_read = false;
    }

    void fifoRead(std::string &Fifo_read, size_t N) {
        char *read_buffer = new char[N];
        if (N > 1024 * 64) { std::cout << "ошибка"; }
        int8_t fifo_read_fd = openFifoRead(Fifo_read.c_str());

        while (run_read) {

            readFifo(fifo_read_fd, read_buffer, N);
            std::cout << "прием от клиента  " << read_buffer << std::endl;
            data+=read_buffer;
        }

        delete read_buffer;
    }

    std::string getData(){
        return  data;
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

    void readFifo(int fifo_fd, char *read_buffer, size_t N) {
        if (fifo_fd == -1) { std::cout << " ошибка: "; }
        memset(read_buffer, 0, N);
        read(fifo_fd, read_buffer, N);

    }



    bool run_write{false};
    bool run_read{false};
    MsgGetter getmsg;
    std::string data = "";
};


#endif
