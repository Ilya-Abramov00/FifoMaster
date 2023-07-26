#ifndef FIFO_H
#define FIFO_H

#include "FifoRead/FifoRead.h"
#include "FifoWrite/FifoWriteQ.h"
#include "FifoWrite/FifoWriteNQ.h"
#include "FifoWrite/FifoIWriter.h"

namespace Ipc {

struct FifoCfg {
	std::string directFile;
	std::string reverseFile;
};

enum class Config { QW, NQW };

class Fifo {
public:
	Fifo(std::unique_ptr<FifoIWriter> fifoWrite, const std::string fdFileNameRead);

	void setConnectionHandlerRead(FifoBase::ConnectionHandler handler);

	void setDisconnectionHandlerRead(FifoBase::ConnectionHandler handler);

	void setConnectionHandlerWrite(FifoBase::ConnectionHandler handler);

	void setDisconnectionHandlerWrite(FifoBase::ConnectionHandler handler);

	void setReadHandler(FifoRead::ReadHandler handler);

	bool const getWaitDisconnectRead() const;

	bool const getWaitConnectRead() const;

	bool const getWaitDisconnectWrite() const;

	bool const getWaitConnectWrite() const;

	void write(const void* data, size_t sizeInBytes);

	void start();

	void stop();

	std::string const getNameRead() const;

	std::string const getNameWrite() const;

	void closeRead();

	void closeWrite();

private:
	std::unique_ptr<FifoIWriter> fifoWrite;
	FifoRead fifoRead;
};
} // namespace Ipc
#endif
