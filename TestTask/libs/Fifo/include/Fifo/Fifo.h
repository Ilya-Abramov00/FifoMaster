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
	FifoRead(Params& params);

	void startRead();

	void stopRead();

private:
	void readFifo();

	uint8_t openFifoRead(char const* FIFO);

	void createFifo(char const* FIFO);

	Params params;

	bool run_read{false};
	char const* FIFO;
	uint8_t fifoFd;
	std::unique_ptr<std::thread> threadReadFifo;
};

class FifoWrite {
public:
	FifoWrite(std::string& fdFileName);

	using MsgGetter = std::function<std::pair<void*, size_t>()>;

	void setMsgGetter(MsgGetter msgGetter);

	void startWrite();

	void stopWrite();


private:
	void writeFifo();

	void writeUser();
	uint8_t openFifoWrite(char const* FIFO);

	void createFifo(char const* FIFO);

	bool run_write{false};
	MsgGetter getmsg;
	char const* FIFO;
	std::queue<std::vector<char>> queue;
	uint8_t fifoFd;
	std::unique_ptr <std::thread> threadWriteFifo;
	std::unique_ptr <std::thread> threadUserWrite; // указатель на поток
};

#endif