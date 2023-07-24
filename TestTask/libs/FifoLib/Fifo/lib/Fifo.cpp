#include "Fifo/Fifo.h"

Fifo::Fifo(const std::string fdFileNameWrite, const std::string fdFileNameRead) :
    fifoRead(fdFileNameRead), fifoWrite(fdFileNameWrite)
{}

void Fifo::setReadHandler(FifoRead::ReadHandler handler)
{
	fifoRead.setReadHandler(std::move(handler));
}

void Fifo::setConnectionHandlerRead(FifoBase::ConnectionHandler handler)
{
	fifoRead.setConnectionHandler(std::move(handler));
}
void Fifo::setDisConnectionHandlerRead(FifoBase::ConnectionHandler handler)
{
	fifoRead.setDisConnectionHandler(std::move(handler));
}

void Fifo::setConnectionHandlerWrite(FifoBase::ConnectionHandler handler)
{
	fifoWrite.setConnectionHandler(std::move(handler));
}
void Fifo::setDisConnectionHandlerWrite(FifoBase::ConnectionHandler handler)
{
	fifoWrite.setDisConnectionHandler(std::move(handler));
}

void Fifo::write(const void* data, size_t sizeInBytes)
{
	fifoWrite.pushData(std::move(data), sizeInBytes);
}
void Fifo::stopRead()
{
	fifoRead.stopRead();
}

void Fifo::stopWrite()
{
	fifoWrite.stopWrite();
}
void Fifo::start()
{
	fifoRead.startRead();
	fifoWrite.startWrite();
}
bool const Fifo::getWaitDisconnectRead() const
{
	return fifoRead.getWaitDisconnect();
}
bool const Fifo::getWaitConnectRead() const
{
	return fifoRead.getWaitConnect();
}
bool const Fifo::getWaitDisconnectWrite() const
{
	return fifoWrite.getWaitDisconnect();
}
bool const Fifo::getWaitConnectWrite() const
{
	return fifoWrite.getWaitConnect();
}
std::string const Fifo::getNameR() const
{
	return fifoRead.getName();
}
std::string const Fifo::getNameW() const
{
	return fifoWrite.getName();
}
