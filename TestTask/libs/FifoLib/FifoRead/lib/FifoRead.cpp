#include "FifoRead/FifoRead.h"

#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <mutex>
#include <queue>

namespace Ipc {
FifoRead::FifoRead(const std::string fdFileName) : params{fdFileName}
{
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
	threadReadFifo = std::make_unique<std::thread>([this]() {
		readFifo();
	});
}

void FifoRead::readFifo()
{
	auto MAXLINE = 1024 * 64;
	std::vector<uint8_t> buffer(MAXLINE);

	while(runRead) {
		connect();

		while(waitConnect && runRead && (fifoFd != -1)) {
			auto flag = read(fifoFd, buffer.data(), MAXLINE);

			if(flag == 0 || flag == -1) {
				waitConnect = false;
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
	waitConnect = false;
}

void FifoRead::setConnectionHandler(ConnectionHandler handler)
{
	params.connectHandler = std::move(handler);
}

void FifoRead::setDisConnectionHandler(ConnectionHandler handler)
{
	params.disconnectHandler = std::move(handler);
}

void FifoRead::setReadHandler(FifoRead::ReadHandler handler)
{
	params.msgHandler = std::move(handler);
}

bool const FifoRead::getWaitConnect() const
{
	return waitConnect;
}

long const& FifoRead::getFifoFd() const
{
	return fifoFd;
}
void FifoRead::connect()
{
	waitOpen = false;
	fifoFd   = openFifo(params.addrRead, 'R');
	waitOpen = true;

	if( runRead && (fifoFd != -1)) {
		waitConnect = true;
		params.connectHandler();
	}
}
} // namespace Ipc