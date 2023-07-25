#include "FifoWrite/FifoWrite.h"



#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>
#include <sys/signal.h>


FifoWrite::FifoWrite(std::string fdFileName)
{
	params.addrRead = fdFileName;
	createFifo(params.addrRead);
}

void FifoWrite::setConnectionHandler(FifoBase::ConnectionHandler handler)
{
	params.connectHandler = std::move(handler);
}

void FifoWrite::setDisConnectionHandler(FifoBase::ConnectionHandler handler)
{
	params.disconnectHandler = std::move(handler);
}

void FifoWrite::startWrite()
{
	if(!params.connectHandler) {
		throw std::runtime_error("callback Write connectHandler not set");
	}
	if(!params.disconnectHandler) {
		throw std::runtime_error("callback Write disconnectHandler not set");
	}
	runWrite = true;

	threadWaitConnectFifo = std::make_unique<std::thread>(std::thread([this]() {
		waitConnectFifo();
	}));
}
void FifoWrite::waitConnectFifo()
{
	fifoFd = openFifo(params.addrRead, 'W');
	if(runWrite) {
		waitConnect     = true;
		params.connectHandler();
	}
}

void FifoWrite::stopWrite()
{
		runWrite = false;
		if(!waitConnect) {
			auto fd = openFifo(params.addrRead.c_str(), 'R');
			close(fd);
		}
		threadWaitConnectFifo->join();
		close(fifoFd);
}


void FifoWrite::pushData(const void* data, size_t sizeN)
{
	if(!data) {
		std::cerr << "\n null ptr is pushData \n";
		return;
	}
	if(waitConnect && runWrite) {
		signal(SIGPIPE, SIG_IGN); // отлавливает сигнал в случае закрытия канала на чтение
		auto flag = write(fifoFd, data, sizeN);
		if(flag == -1) {
			waitDisConnect = true;
			params.disconnectHandler();
			return;
		}
	}
}
bool const FifoWrite::getWaitDisconnect() const
{
	return waitDisConnect;
}
std::string const FifoWrite::getName() const
{
	return params.addrRead;
}
long const& FifoWrite::getFifoFd() const
{
	return fifoFd;
}
bool const FifoWrite::getWaitConnect() const
{
	return waitConnect;
}
