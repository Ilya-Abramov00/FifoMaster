#include "FifoWrite/FifoWriteDirect.h"

#include <unistd.h>
#include <iostream>
#include <string>
#include <list>
#include <mutex>
#include <sys/signal.h>
#include <future>
namespace Ipc {

WriteDirectImpl::WriteDirectImpl(std::string fdFileName, size_t waitTimeConnectMilliSeconds, size_t waitTimeReconnectMilliSeconds) : params{fdFileName}
{
	if(!waitTimeConnectMilliSeconds) {
		throw std::runtime_error(" not waitTime set");
	}
	if(!waitTimeReconnectMilliSeconds) {
		throw std::runtime_error(" not waitTime set");
	}
	params.waitConnnectTimeMilliSeconds =waitTimeConnectMilliSeconds;
	params.waitReconnectTimeMilliSeconds =waitTimeReconnectMilliSeconds;
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
		throw std::runtime_error("callback Write connectHandler not set");
	}
	if(!params.disconnectHandler) {
		throw std::runtime_error("callback Write disconnectHandler not set");
	}
	waitConnectFifo(params.waitConnnectTimeMilliSeconds);
}

void WriteDirectImpl::waitConnectFifo(size_t time)
{
	std::future t = std::async([this]() {
		waitOpen = false;
		fifoFd   = openFifo(params.addrRead, 'W');
		waitOpen = true;
	});

	t.wait_for(std::chrono::milliseconds(time));

	if(waitOpen && fifoFd != -1) {
		waitConnect = true;
		params.connectHandler();
	}
}

void WriteDirectImpl::pushData(const void* data, size_t sizeN)
{
	if(!waitConnect) {
		waitConnectFifo(params.waitReconnectTimeMilliSeconds);
		if(!waitConnect) {
			throw std::runtime_error("write close Fifo");
		}
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