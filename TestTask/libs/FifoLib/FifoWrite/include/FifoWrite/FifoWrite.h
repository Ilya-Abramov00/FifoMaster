#ifndef FIFOWRITE_H
#define FIFOWRITE_H

#include "FifoBase/FifoBase.h"

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

class FifoWrite : protected FifoBase {
public:
	void setConnectionHandler(FifoBase::ConnectionHandler handler);
	void setDisConnectionHandler(FifoBase::ConnectionHandler handler);

	void startWrite();
	void stopWrite();

	FifoWrite(const std::string fdFileName);

	void pushData(const void* data, size_t sizeN);

	bool const getWaitDisconnect() const
	{
		return waitDisConnect;
	}
	bool const getWaitConnect() const
	{
		return waitConnect;
	}

	std::string const getName()const{
		return params.addrRead;
	}
	long const& getFifoFd()const{
		return fifoFd;
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
	long fifoFd = -1;
	std::queue<std::vector<uint8_t>> queue;
	std::mutex mtx;
	std::unique_ptr<std::thread> threadWriteFifo;
	std::unique_ptr<std::thread> threadWaitConnectFifo;
};

#endif
