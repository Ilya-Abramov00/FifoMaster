#include "Fifo/Fifo.h"


void FifoRead::start_read() {
    run_read = true;
}

void FifoRead::stop_read() {
    run_read = false;
}

void FifoRead::fifoRead(std::string &Fifo_read_0, size_t N) {
    auto Fifo_read = Fifo_read_0.c_str();
    char *read_buffer = new char[N];

    if (N > 1024 * 64) throw std::runtime_error("fail very big N ");

    int8_t fifo_read_fd = openFifoRead(Fifo_read);

    while (run_read) {

        readFifo(fifo_read_fd, read_buffer, N);
        std::cout << "получил сообщение:  " << read_buffer << std::endl;
        data += read_buffer;
    }

    delete read_buffer;
}

std::string FifoRead::getData() {
    return data;
}

int FifoRead::openFifoRead(const char *FIFO) {
    int fd = open(FIFO, O_RDONLY, 0);
    if (-1 == fd) { throw std::runtime_error("fail openFifoWrite"); }

    return fd;
}

void FifoRead::readFifo(int fifo_fd, char *read_buffer, size_t N) {
    if (fifo_fd == -1) { throw std::runtime_error("fail ReadFifo"); }
    memset(read_buffer, 0, N);
    read(fifo_fd, read_buffer, N);

}

void FifoWrite::setMsgGetter(FifoWrite::MsgGetter msgGetter) {
    getmsg = std::move(msgGetter);
}

void FifoWrite::start_write() {
    if (!getmsg) {
        throw std::runtime_error("callback for msg getting not set");
    }
    run_write = true;
}

void FifoWrite::stop_write() {
    run_write = false;
}

void FifoWrite::fifoWrite(std::string &Fifo_write_q) {
    auto Fifo_write = Fifo_write_q.c_str();

    createFifo(Fifo_write);
    int8_t fifo_write_fd = openFifoWrite(Fifo_write);
    while (run_write) {
        writeFifo(fifo_write_fd, getmsg().c_str());
    }
    unlink(Fifo_write);

}

int FifoWrite::openFifoWrite(const char *FIFO) {
    int fd = open(FIFO, O_WRONLY, 0);
    if (-1 == fd) { throw std::runtime_error("fail openFifoWrite"); }

    return fd;
}

void FifoWrite::writeFifo(int fifo_fd, const char *data) {
    if (fifo_fd == -1) { throw std::runtime_error("fail WriteFifo"); }
    write(fifo_fd, data, strlen(data));

}

void FifoWrite::createFifo(const char *FIFO) {
    if ((mkfifo(FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
        std::cout << FIFO << '\n';
        throw std::runtime_error(" fail createFifo ");
    }
}
