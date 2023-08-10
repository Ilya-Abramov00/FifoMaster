#include "FifoWrite/FifoWriteNQ.h"

#include <unistd.h>
#include <iostream>
#include <string>
#include <list>
#include <mutex>
#include <sys/signal.h>
#include <future>
namespace Ipc {

NQWriteImpl::NQWriteImpl(std::string fdFileName, size_t waitMilliSeconds) : params{fdFileName}
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
	waitConnectFifo();
}

void NQWriteImpl::waitConnectFifo()
{
	std::future t = std::async([this]() {
		waitOpen = false;
		fifoFd   = openFifo(params.addrRead, 'W');
		waitOpen = true;
	});

	t.wait_for(std::chrono::seconds(1));
	if(waitOpen) {
		waitConnect = true;
		params.connectHandler();
	}
}

void NQWriteImpl::stopWrite()
{
	if(!waitOpen) {
		auto fd = openFifo(params.addrRead.c_str(), 'R');
		close(fd);
	}

	close(fifoFd);
}

void NQWriteImpl::pushData(const void* data, size_t sizeN)
{
	if(!waitConnect) {
		throw std::runtime_error("write no connect");
	}
	if(!data) {
		std::cerr << "\n null ptr is pushData \n";
		return;
	}
	signal(SIGPIPE, SIG_IGN); // отлавливает сигнал в случае закрытия канала на чтение
	auto flag = write(fifoFd, data, sizeN);
	if(flag == -1) {
		waitConnect = false;
		params.disconnectHandler();

	}
}

long const& NQWriteImpl::getFifoFd() const
{
	return fifoFd;
}

bool NQWriteImpl::getWaitConnect() const
{
	return waitConnect;
}
} // namespace Ipc