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
#include <chrono>
#include <thread>
#include <list>
#include <mutex>
#include <queue>
#include <memory>
// class FifoException : public std::runtime_error {
// public:
//	FifoException(std::string const& msg);
// };
//
// class FifoWriteException : FifoException {
// public:
//	FifoWriteException(std::string const& msg);
// };
//
// class FifoReadException : FifoException {
// public:
//	FifoReadException(std::string const& msg);
// };
// class FifoAniException : FifoException {
// public:
//	FifoAniException(std::string const& msg);
// };

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IRUSR)
#define MAXLINE 64 * 1024

using Data              = std::vector<uint8_t>;
using ReadHandler       = std::function<void(Data&&)>;
using ConnectionHandler = std::function<void()>;

struct Params {
	std::string addrRead;
	ReadHandler  msgHandler;
	ConnectionHandler connectHandler;
};


class FifoRead {
public:
	FifoRead(const Params& params);

	void startRead();
	void stopRead();
	~FifoRead();

private:
	void waitConnectFifo();
	void readFifo();
	uint8_t openFifoRead();
	void createFifo();

	Params params;
	bool runRead{false};
	bool waitConnect{false};
	char const* FIFO;
	uint8_t fifoFd = -1;
	std::unique_ptr<std::thread> threadWaitConnectFifo;
	std::unique_ptr<std::thread> threadReadFifo;
};

class FifoWrite {
public:
	FifoWrite(const std::string& fdFileName);

	void pushData(void* data, size_t sizeN);

	void startWrite();

	void stopWrite();


private:
	void writeFifo();
	uint8_t openFifoWrite();
	void createFifo();

	bool runWrite{false};
	char const* FIFO;
	std::queue<std::vector<uint8_t>> queue;
	uint8_t fifoFd = -1;
	std::mutex mtx;
	std::unique_ptr<std::thread> threadWriteFifo;
};

class Server{
public:

private:
};
#endif
