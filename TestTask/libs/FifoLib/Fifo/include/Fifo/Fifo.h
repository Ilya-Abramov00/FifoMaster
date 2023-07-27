#ifndef FIFO_H
#define FIFO_H

#include "FifoRead/FifoRead.h"
#include "FifoWrite/FifoWriteQ.h"
#include "FifoWrite/FifoWriteNQ.h"
#include "FifoWrite/IFifoWriter.h"

namespace Ipc {

struct FifoCfg {
	std::string directFile;
	std::string reverseFile;
};

enum class Config { QW, NQW };

class Fifo {
public:
	Fifo(std::unique_ptr<IFifoWriter> fifoWrite, const std::string fdFileNameRead);

	void setConnectionHandlerRead(ConnectionHandler handler);

	void setDisconnectionHandlerRead(ConnectionHandler handler);

	void setConnectionHandlerWrite(ConnectionHandler handler);

	void setDisconnectionHandlerWrite(ConnectionHandler handler);

	void setReadHandler(FifoRead::ReadHandler handler);

	bool const getWaitDisconnectRead() const;

	bool const getWaitConnectRead() const;

	bool const getWaitDisconnectWrite() const;

	bool const getWaitConnectWrite() const;

	void write(const void* data, size_t sizeInBytes);

	void start();

	void stop();

	void closeRead();

	void closeWrite();

private:
	std::unique_ptr<IFifoWriter> fifoWrite;
	FifoRead fifoRead;
};
} // namespace Ipc
#endif
