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
public:
	using ConnectionHandler = std::function<void()>;

protected:
	long openFifo(const std::string fdFileName, const char flag);
	void createFifo(const std::string fdFileName);
};

class FifoRead : protected FifoBase {
public:
	using Data        = std::vector<uint8_t>;
	using ReadHandler = std::function<void(Data&&)>;

	FifoRead(const std::string fdFileName);
	void startRead();
	void stopRead();

	void setConnectionHandler(FifoBase::ConnectionHandler handler);
	void setDisConnectionHandler(FifoBase::ConnectionHandler handler);

	void setReadHandler(ReadHandler handler);
	~FifoRead();
	bool const getWaitDisConnect() const
	{
		return waitDisConnect;
	}
	bool const getWaitConnect() const
	{
		return waitConnect;
	}

private:
	void waitConnectFifo();
	void readFifo();

	struct Params {
		std::string addrRead;
		ReadHandler msgHandler;
		ConnectionHandler connectHandler;
		ConnectionHandler disconnectHandler;
	};
	Params params;
	bool runRead{false};
	bool waitConnect{false};
	bool waitDisConnect{false};
	long fifoReadFd = -1;
	std::unique_ptr<std::thread> threadWaitConnectFifo;
	std::unique_ptr<std::thread> threadReadFifo;
};

class FifoWrite : protected FifoBase {
public:
	void setConnectionHandler(FifoBase::ConnectionHandler handler);
	void setDisConnectionHandler(FifoBase::ConnectionHandler handler);

	void startWrite();
	void stopWrite();

	FifoWrite(const std::string fdFileName);

	void pushData(const void* data, size_t sizeN);

	bool const getWaitDisConnect() const
	{
		return waitDisConnect;
	}
	bool const getWaitConnect() const
	{
		return waitConnect;
	}

private:
	void waitConnectFifo();
	void writeFifo();

	struct Params {
		std::string addrRead;
		ConnectionHandler connectHandler;
		ConnectionHandler disconnectHandler;
	};
	Params params;
	bool runWrite{false};
	bool waitConnect{false};
	bool waitDisConnect{false};
	std::queue<std::vector<uint8_t>> queue;
	long fifoFd = -1;
	std::mutex mtx;
	std::unique_ptr<std::thread> threadWriteFifo;
	std::unique_ptr<std::thread> threadWaitConnectFifo;
};

class Fifo {
public:
	Fifo(const std::string fdFileNameWrite, const std::string fdFileNameRead);

	void setConnectionHandlerRead(FifoBase::ConnectionHandler handler);
	void setDisConnectionHandlerRead(FifoBase::ConnectionHandler handler);

	void setConnectionHandlerWrite(FifoBase::ConnectionHandler handler);
	void setDisConnectionHandlerWrite(FifoBase::ConnectionHandler handler);

	void setReadHandler(FifoRead::ReadHandler handler);
	bool const getWaitDisConnectRead() const
	{
		return fifoRead.getWaitDisConnect();
	}
	bool const getWaitConnectRead() const
	{
		return fifoRead.getWaitConnect();
	}
	bool const getWaitDisConnectWrite() const
	{
		return fifoWrite.getWaitDisConnect();
		;
	}
	bool const getWaitConnectWrite() const
	{
		return fifoWrite.getWaitConnect();
	}

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

	void write(std::shared_ptr<Fifo> object, const void* data, size_t sizeInBytes);

	void start();
	void stop();
	ConnectionId connectionId;

private:
	void getter(FifoRead::Data&& data);

	void logicConnect(std::shared_ptr<Fifo> object);
	void logicDisConnect(std::shared_ptr<Fifo> object);

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
