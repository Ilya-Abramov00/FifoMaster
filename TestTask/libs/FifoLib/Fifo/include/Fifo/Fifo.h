#ifndef FIFO_H
#define FIFO_H


#include "FifoRead/FifoRead.h"
#include "FifoWrite/FifoWrite.h"

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

	std::string const getNameR()const{
		return fifoRead.getName();
	}
	std::string const getNameW()const{
		return fifoWrite.getName();
	}
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
