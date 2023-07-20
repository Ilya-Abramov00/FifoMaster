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

using Data              = std::vector<uint8_t>;
using ReadsHandler      = std::function<void(Data&&)>;
using ConnectionHandler = std::function<void()>;

class FifoRead {
public:
	void startRead();
	void stopRead();
	const bool getBooLWaitConnect() const;

	FifoRead(const std::string fdFileName);

	~FifoRead();

	void setConnectionHandler(ConnectionHandler handler);
	void setReadHandler(ReadsHandler handler);

private:
	void waitConnectFifo();
	void readFifo();
	long openFifoRead();
	void createFifo();

	struct Params {
		std::string addrRead;
		ReadsHandler msgHandler;
		ConnectionHandler connectHandler;
	};
	Params params;
	bool runRead{false};
	bool waitConnect{false};
	long fifoReadFd = -1;
	std::unique_ptr<std::thread> threadWaitConnectFifo;
	std::unique_ptr<std::thread> threadReadFifo;
};

class FifoWrite {
public:
	void startWrite();
	void stopWrite();

	FifoWrite(const std::string fdFileName);

	void pushData(const void* data, size_t sizeN);

	const bool getWaitConnect() const;

private:
	void waitConnectFifo();
	void writeFifo();
	long openFifoWrite();
	void createFifo();

	bool runWrite{false};
	bool waitConnect{false};
	const std::string fdFileName;
	std::queue<std::vector<uint8_t>> queue;
	long fifoFd = -1;
	std::mutex mtx;
	std::unique_ptr<std::thread> threadWriteFifo;
	std::unique_ptr<std::thread> threadWaitConnectFifo;
};

class Fifo {
public:
	Fifo(const std::string fdFileNameWrite, const std::string fdFileNameRead);

	void setConnectionHandler(ConnectionHandler handler);
	void setReadHandler(ReadsHandler handler);

	void write(const void* data, size_t sizeInBytes);

	void start()
	{
		fifoRead.startRead();
		fifoWrite.startWrite();
	}
	void stop()
	{
		fifoWrite.stopWrite();
		fifoRead.stopRead();
	}

private:
	FifoWrite fifoWrite;
	FifoRead fifoRead;
};

#endif
