#ifndef FIFO_H
#define FIFO_H

#include "FifoRead/FifoRead.h"
#include "FifoWrite/FifoWriteQ.h"
#include "FifoWrite/FifoWriteDirect.h"
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

	bool getWaitConnectRead() const;

	bool getWaitConnectWrite() const;

	void write(const void* data, size_t sizeInBytes);

	void start();

	void stop();

	void closeRead();

	void closeWrite();

	void connectWrite(){
		fifoWrite->connect();
	}
	void connectRead(){
		fifoRead.connect();
	}
void recoonectTrue(){
		fifoRead.recoonectTrue();
		fifoWrite->recoonectTrue();
}
private:
	std::unique_ptr<IFifoWriter> fifoWrite;
	FifoRead fifoRead;
	enum class State { start, stop };
	State state = State::stop;
};
} // namespace Ipc
#endif
