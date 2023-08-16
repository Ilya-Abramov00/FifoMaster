#include "FifoWrite/FifoWriteDirect.h"

#include <unistd.h>
#include <iostream>
#include <string>
#include <list>
#include <mutex>
#include <sys/signal.h>
#include <future>
namespace Ipc {

WriteDirectImpl::WriteDirectImpl(std::string fdFileName, size_t waitTimeConnectMilliSeconds,
                                 size_t waitTimeReconnectMilliSeconds) :
    params{fdFileName}
{
	if(!waitTimeConnectMilliSeconds) {
		throw std::runtime_error(" not waitTime set");
	}
	if(!waitTimeReconnectMilliSeconds) {
		throw std::runtime_error(" not waitTime set");
	}
	params.waitConnnectTimeMilliSeconds  = waitTimeConnectMilliSeconds;
	params.waitReconnectTimeMilliSeconds = waitTimeReconnectMilliSeconds;
	createFifo(params.addrRead);
}

void WriteDirectImpl::setConnectionHandler(ConnectionHandler handler)
{
	params.connectHandler = std::move(handler);
}

void WriteDirectImpl::setDisconnectionHandler(ConnectionHandler handler)
{
	params.disconnectHandler = std::move(handler);
}

void WriteDirectImpl::startWrite()
{
	if(!params.connectHandler) {
		throw std::runtime_error("callback Write newHandler not set");
	}
	if(!params.disconnectHandler) {
		throw std::runtime_error("callback Write closeHandler not set");
	}
	connect();
}

void WriteDirectImpl::connect()
{
	std::future f = std::async([this]() {
		waitOpen = false;
		fifoFd   = openFifo(params.addrRead, 'W');
		waitOpen = true;
	});

	f.wait_for(std::chrono::milliseconds(params.waitConnnectTimeMilliSeconds));

	if(waitOpen && fifoFd != -1) {
		waitConnect = true;
		params.connectHandler();
	}
}

void WriteDirectImpl::pushData(const void* data, size_t sizeN)
{
	if(!waitConnect) {
		throw std::runtime_error("write close Fifo");
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

void WriteDirectImpl::stopWrite()
{
	if(!waitOpen) {
		auto fd = openFifo(params.addrRead.c_str(), 'R');
		close(fd);
	}

	close(fifoFd);
	waitConnect = false;
	params.disconnectHandler();
}

long const& WriteDirectImpl::getFifoFd() const
{
	return fifoFd;
}

bool WriteDirectImpl::getWaitConnect() const
{
	return waitConnect;
}
} // namespace Ipc