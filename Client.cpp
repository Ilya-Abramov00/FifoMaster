
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
const char *FIFO3 = "/home/ilya/Загрузки/Pipe/fifo3";
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR)
#define MAXLINE 4


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
        client_write_fd = OpenFifoWrite(FIFO1);
        client_count_write_fd = OpenFifoWrite(FIFO2);

        while (run) {
            auto msg = getmsg();
            client_buf_count[0] = 1;
            client_buf_count[1] = 0;

            write(client_count_write_fd, client_buf_count, 2);

            WriteFifo(client_write_fd, client_buf);
            std::cout << "отправка серверу " << client_buf << std::endl;

            if (client_buf[0] == '1') { break; }
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
        read(client_fd, buffer, sizeof(buffer));

    }


    int8_t client_read_fd = -1;
    int8_t client_write_fd = -1;
    int8_t client_count_write_fd = -1;

    char client_buf[MAXLINE] = {0};
    char client_buf_count[2] = {0, 0};
    std::string data_buffer = "";
    bool run{false};
    MsgGetter getmsg;
};

int main() {


    std::cout << "клиент" << std::endl;

    ClientPipe a;
    auto getter = []() {
        std::string ret;
        //ret = " hello! ";
        std::cin >> ret;
        return ret;

    };
    a.setMsgGetter(getter);
    a.start();

    a.pipeWrite();

    std::cout << "сервер завершил отправку" << std::endl;

    return 0;
}
