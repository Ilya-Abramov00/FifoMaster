#include "FifoRead/FifoRead.h"

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
	runRead               = true;
	threadWaitConnectFifo = std::make_unique<std::thread>(std::thread([this]() {
		waitConnectFifo();
	}));
}

void FifoRead::waitConnectFifo()
{
	fifoReadFd = openFifo(params.addrRead, 'R');
	if(runRead) {
		params.connectHandler(); // соединение проиошло
		waitConnect    = true;
		threadReadFifo = std::make_unique<std::thread>(std::thread([this]() {
			readFifo();
		}));
	}
}

void FifoRead::readFifo()
{
	auto MAXLINE = 1024 * 64;
	std::vector<uint8_t> buffer(MAXLINE);
	if(fifoReadFd == -1) {
		throw std::runtime_error(" fail openFifo ");
	}
	while(runRead) {
		auto flag = read(fifoReadFd, buffer.data(), MAXLINE);
		if(flag == 0) {
			waitDisConnect = true;
			params.disconnectHandler();
			return;
		}
		if(flag == MAXLINE) {
			params.msgHandler(std::move(buffer));
		}
		else {
			params.msgHandler(std::vector<uint8_t>(buffer.data(), buffer.data() + flag));
		}
	}
}
void FifoRead::stopRead()
{
	runRead = false;
	auto fd = openFifo(params.addrRead.c_str(), 'W');

	threadWaitConnectFifo->join();
	close(fd);
	close(fifoReadFd);
	if(waitConnect) {
		threadReadFifo->join();
	}
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

FifoRead::~FifoRead()
{
	unlink(params.addrRead.c_str());
}
