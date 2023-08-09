#include "FifoWrite/FifoWriteNQ.h"

#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <mutex>
#include <sys/signal.h>

namespace Ipc {

NQWriteImpl::NQWriteImpl(std::string fdFileName) :params{ fdFileName}
{
	createFifo(params.addrRead);
}

void NQWriteImpl::setConnectionHandler(ConnectionHandler handler)
{
	params.connectHandler = std::move(handler);
}

void NQWriteImpl::setDisConnectionHandler(ConnectionHandler handler)
{
	params.disconnectHandler = std::move(handler);
}

void NQWriteImpl::startWrite()
{
	if(!params.connectHandler) {
		throw std::runtime_error("callback Write connectHandler not set");
	}
	if(!params.disconnectHandler) {
		throw std::runtime_error("callback Write disconnectHandler not set");
	}
	runWrite = true;

	threadWaitConnectFifo = std::make_unique<std::thread>([this]() {
		waitConnectFifo();
	});
}

void NQWriteImpl::waitConnectFifo()
{
	// while(){}чтобы многорузовой сделать
	fifoFd = openFifo(params.addrRead, 'W');
	if(runWrite) {
		waitConnect = true;
		params.connectHandler();
	}
}

void NQWriteImpl::stopWrite()
{
	runWrite = false;
	if(!waitConnect) {
		auto fd = openFifo(params.addrRead.c_str(), 'R');
		close(fd);
	}
	threadWaitConnectFifo->join();
	close(fifoFd);
}

void NQWriteImpl::pushData(const void* data, size_t sizeN)
{
	if(waitConnect && runWrite) {
		if(!data) {
			std::cerr << "\n null ptr is pushData \n";
			return;
		}
		signal(SIGPIPE, SIG_IGN); // отлавливает сигнал в случае закрытия канала на чтение
		auto flag = write(fifoFd, data, sizeN);
		if(flag == -1) {
			waitDisConnect = true;
			params.disconnectHandler();
			return;
		}
	}
	else {
		throw std::runtime_error("Write in close Fifo");
	}
}

bool const NQWriteImpl::getWaitDisconnect() const
{
	return waitDisConnect;
}

long const& NQWriteImpl::getFifoFd() const
{
	return fifoFd;
}

bool const NQWriteImpl::getWaitConnect() const
{
	return waitConnect;
}
} // namespace Ipc