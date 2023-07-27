#include "FifoRead/FifoRead.h"

#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <mutex>
#include <queue>

namespace Ipc {
FifoRead::FifoRead(const std::string fdFileName)
{
	params.addrRead = fdFileName;
	createFifo(params.addrRead);
}

void FifoRead::startRead()
{
	if(!params.msgHandler) {
		throw std::runtime_error("callback ReadHandler not set");
	}
	if(!params.connectHandler) {
		throw std::runtime_error("callback Read connectHandler not set");
	}
	if(!params.disconnectHandler) {
		throw std::runtime_error("callback Read disconnectHandler not set");
	}
	runRead        = true;
	threadReadFifo = std::make_unique<std::thread>(std::thread([this]() {
		readFifo();
	}));
}

void FifoRead::readFifo()
{
	auto MAXLINE = 1024 * 64;
	std::vector<uint8_t> buffer(MAXLINE);

	while(runRead) {
		waitOpen = false;
		fifoFd   = openFifo(params.addrRead, 'R');
		if(fifoFd == -1) {
			throw std::runtime_error(" fail openFifo ");
		}
		waitOpen       = true;
		waitDisConnect = false;
		waitConnect    = true;
		params.connectHandler();

		while(waitConnect && runRead) {
			auto flag = read(fifoFd, buffer.data(), MAXLINE);
			if(flag == 0) {
				waitConnect    = false;
				waitDisConnect = true;
				params.disconnectHandler();
				break;
			}
			else if(flag == MAXLINE) {
				params.msgHandler(std::move(buffer));
			}
			else {
				params.msgHandler(std::vector<uint8_t>(buffer.data(), buffer.data() + flag));
			}
		}
	}
}

void FifoRead::stopRead()
{
	runRead     = false;
	waitConnect = false;

	if(!waitOpen) {
		auto fd = openFifo(params.addrRead.c_str(), 'W');

		close(fd);
	}
	close(fifoFd);

	threadReadFifo->join();
	waitDisConnect = false;
}

void FifoRead::setConnectionHandler(FifoBase::ConnectionHandler handler)
{
	params.connectHandler = std::move(handler);
}

void FifoRead::setDisConnectionHandler(FifoBase::ConnectionHandler handler)
{
	params.disconnectHandler = std::move(handler);
}

void FifoRead::setReadHandler(FifoRead::ReadHandler handler)
{
	params.msgHandler = std::move(handler);
}

bool const FifoRead::getWaitDisconnect() const
{
	return waitDisConnect;
}

bool const FifoRead::getWaitConnect() const
{
	return waitConnect;
}

long const& FifoRead::getFifoFd() const
{
	return fifoFd;
}
} // namespace Ipc