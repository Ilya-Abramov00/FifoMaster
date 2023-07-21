#ifndef FIFO_H
#define FIFO_H

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

#include "Fifo/FifoBase.h"

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
