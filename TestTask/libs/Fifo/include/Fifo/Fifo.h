#ifndef COMPLEX_H
#define COMPLEX_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <functional>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR)
#define MAXLINE 64 * 1024

class FifoRead {
public:
	FifoRead(std::string &Fifo_write_q);
	void start_read();

	void stop_read();

	void readFifo(std::string& data, size_t N);

private:
	int openFifoRead(char const* FIFO);

	void readFifo(uint8_t fifo_fd, char* read_buffer, size_t N);
	void createFifo(char const* FIFO);

	bool run_read{false};
	char const* FIFO;
};

class FifoWrite {
public:
	FifoWrite(std::string &Fifo_write);
	using MsgGetter = std::function<std::string()>;

	void setMsgGetter(MsgGetter msgGetter);

	void start_write();

	void stop_write();

	void writeFifo();

private:
	int openFifoWrite(char const* FIFO);

	void writeFifo(int fifo_fd, char const* data);

	void createFifo(char const* FIFO);

	bool run_write{false};
	MsgGetter getmsg;
	char const* FIFO;
};

#endif