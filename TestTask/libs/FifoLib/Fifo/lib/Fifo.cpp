#include "Fifo/Fifo.h"

namespace Ipc {
Fifo::Fifo(std::unique_ptr<IFifoWriter> fifoWrite, const std::string fdFileNameRead) :
    fifoRead(fdFileNameRead), fifoWrite(std::move(fifoWrite))
{}

void Fifo::setReadHandler(FifoRead::ReadHandler handler)
{
	fifoRead.setReadHandler(std::move(handler));
}

void Fifo::setConnectionHandlerRead(ConnectionHandler handler)
{
	fifoRead.setConnectionHandler(std::move(handler));
}

void Fifo::setDisconnectionHandlerRead(ConnectionHandler handler)
{
	fifoRead.setDisConnectionHandler(std::move(handler));
}

void Fifo::setConnectionHandlerWrite(ConnectionHandler handler)
{
	fifoWrite->setConnectionHandler(std::move(handler));
}

void Fifo::setDisconnectionHandlerWrite(ConnectionHandler handler)
{
	fifoWrite->setDisConnectionHandler(std::move(handler));
}

void Fifo::write(const void* data, size_t sizeInBytes)
{
	fifoWrite->pushData(data, sizeInBytes);
}

void Fifo::stop()
{
	if(state == State::start) {
		state = State::stop;
		fifoWrite->stopWrite();
		fifoRead.stopRead();
	}
}

void Fifo::start()
{
	if(state == State::stop) {
		state=State::start;
		fifoRead.startRead();
		fifoWrite->startWrite();
	}
}


bool  Fifo::getWaitConnectRead() const
{
	return fifoRead.getWaitConnect();
}

bool  Fifo::getWaitConnectWrite() const
{
	return fifoWrite->getWaitConnect();
}

void Fifo::closeRead()
{
	close(fifoRead.getFifoFd());
}

void Fifo::closeWrite()
{
	close(fifoWrite->getFifoFd());
}
} // namespace Ipc