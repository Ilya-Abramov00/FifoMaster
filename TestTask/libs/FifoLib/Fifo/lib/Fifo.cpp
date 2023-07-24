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
void Fifo::stop()
{
	fifoWrite.stopWrite();
	fifoRead.stopRead();
}

void Fifo::start()
{
	fifoRead.startRead();
	fifoWrite.startWrite();
}
