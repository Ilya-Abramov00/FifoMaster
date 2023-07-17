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

struct Params {
	std::string fdFileName;
	size_t dataUnitSize;
	size_t timeToWaitDataNanoSeconds; // for sleep curr thread
	std::function<void(void* data, size_t szInBytes)> msgHandler;
};

class FifoRead {
public:
	FifoRead(const Params& params);

	void startRead();

	void stopRead();
	~FifoRead();

private:
	void readFifo();
	uint8_t openFifoRead();
	void createFifo();

	Params params;
	bool runRead{false};
	char const* FIFO;
	uint8_t fifoFd = -1;
	std::unique_ptr<std::thread> threadReadFifo;
};

class FifoWrite {
public:
	FifoWrite(const std::string& fdFileName);

	void pushData(void* data, size_t sizeN);

	void startWrite();

	void stopWrite();

	~FifoWrite();

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

#endif