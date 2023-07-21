#ifndef COMPLEX_H
#define COMPLEX_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
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
#include <unistd.h>
#include <sys/signal.h>

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

class FifoBase {
protected:
	long openFifo(const std::string fdFileName, const char flag);
	void createFifo(const std::string fdFileName);
};

class FifoRead : protected FifoBase {
public:
	using Data              = std::vector<uint8_t>;
	using ReadHandler       = std::function<void(Data&&)>;
	using ConnectionHandler = std::function<void()>;

	FifoRead(const std::string fdFileName);
	void startRead();
	void stopRead();

	void setConnectionHandler(ConnectionHandler handler);
	void setReadHandler(ReadHandler handler);
	~FifoRead();

private:
	void waitConnectFifo();
	void readFifo();

	struct Params {
		std::string addrRead;
		ReadHandler msgHandler;
		ConnectionHandler connectHandler;
	};
	Params params;
	bool runRead{false};
	bool waitConnect{false};
	long fifoReadFd = -1;
	std::unique_ptr<std::thread> threadWaitConnectFifo;
	std::unique_ptr<std::thread> threadReadFifo;
};

class FifoWrite : protected FifoBase {
public:
	void startWrite();
	void stopWrite();

	FifoWrite(const std::string fdFileName);

	void pushData(const void* data, size_t sizeN);

private:
	void waitConnectFifo();
	void writeFifo();

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

	void setConnectionHandler(FifoRead::ConnectionHandler handler);
	void setReadHandler(FifoRead::ReadHandler handler);

	void write(const void* data, size_t sizeInBytes);

	void start();
	void stop();

private:
	FifoWrite fifoWrite;
	FifoRead fifoRead;
};

class Server {
public:
	using ConnectionId          = std::unordered_map<std::string, std::shared_ptr<Fifo>>;
	using ConnChangeHandler     = std::function<void(ConnectionId)>;
	using ReadHandler           = std::function<void(ConnectionId, FifoRead::Data&&)>;
	using IdDistributionHandler = std::function<ConnectionId()>;

	Server(const std::vector<std::string>& nameChannelsFifo);

	void setReadHandler(ReadHandler h);

	void setNewConnectionHandler(ConnChangeHandler h);
	void setCloseConnectionHandler(ConnChangeHandler h);

	void start();
	void stop();

private:
	void getter(FifoRead::Data&& data);
	void connect();

	ConnectionId connectionId;
	const std::vector<std::string>& nameChannelsFifo;

	ReadHandler readHandler;

	ConnChangeHandler newHandler;
	ConnChangeHandler closeHandler;
};

// virtual ~Server() = default;
// virtual void start()                                             = 0;
// virtual void stop()                                              = 0;
// virtual void write(ConnectionId id, const void* data, size_t sz) = 0;
// virtual void disconnect(ConnectionId id)                         = 0;
// void setNewConnectionHandler(ConnChangeHandler h)
//{
//	newHandler = h;
// }
// void setCloseConnectionHandler(ConnChangeHandler h)
//{
//	closeHandler = h;
// }

// virtual void setIdDistributionHandler(IdDistributionHandler h) = 0;
#endif
