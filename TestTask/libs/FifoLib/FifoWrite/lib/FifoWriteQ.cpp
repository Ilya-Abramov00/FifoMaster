#include "FifoWrite/FifoWriteQ.h"

#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <mutex>
#include <queue>
#include <sys/signal.h>

namespace Ipc {
QWriteImpl::QWriteImpl(std::string fdFileName)
{
	params.addrRead = fdFileName;
	createFifo(params.addrRead);
}

void QWriteImpl::setConnectionHandler(FifoBase::ConnectionHandler handler)
{
	params.connectHandler = std::move(handler);
}

void QWriteImpl::setDisConnectionHandler(FifoBase::ConnectionHandler handler)
{
	params.disconnectHandler = std::move(handler);
}

void QWriteImpl::startWrite()
{
	if(!params.connectHandler) {
		throw std::runtime_error("callback Write connectHandler not set");
	}
	if(!params.disconnectHandler) {
		throw std::runtime_error("callback Write disconnectHandler not set");
	}
	runWrite = true;

	threadWriteFifo = std::make_unique<std::thread>(std::thread([this]() {
		writeFifo();
	}));
}

void QWriteImpl::writeFifo()
{
	fifoFd = openFifo(params.addrRead, 'W');
	if(fifoFd == -1) {
		throw std::runtime_error(" fail openFifo ");
	}

	waitConnect = true;
	params.connectHandler();

	while(runWrite) {
		{
			std::lock_guard<std::mutex> mtx_0(mtx);
			if(!queue.empty()) {
				signal(SIGPIPE, SIG_IGN); // отлавливает сигнал в случае закрытия канала на чтение
				auto flag = write(fifoFd, queue.front().data(), queue.front().size());
				if(flag == -1) {
					waitDisConnect = true;
					waitConnect    = false;
					params.disconnectHandler();
					break;
				}
				queue.pop();
			}
		}
	}
}

void QWriteImpl::pushData(const void* data, size_t sizeN)
{
	if(!data) {
		std::cerr << "\n null ptr is pushData \n";
		return;
	}
	auto ptr = reinterpret_cast<const uint8_t*>(data);
	std::vector<uint8_t> buffer(ptr, ptr + sizeN);
	{
		std::lock_guard<std::mutex> mtx_0(mtx);
		queue.push(std::move(buffer));
	}
}

void QWriteImpl::stopWrite()
{
	runWrite = false;
	if(!waitConnect) {
		auto fd = openFifo(params.addrRead.c_str(), 'R');
		close(fd);
	}
	close(fifoFd);

	threadWriteFifo->join();

	if(queue.size()) {
		std::cerr << getName() << "в очереди остались неотправленные сообщения\n";
	}
}

bool const QWriteImpl::getWaitDisconnect() const
{
	return waitDisConnect;
}

std::string const QWriteImpl::getName() const
{
	return params.addrRead;
}

long const& QWriteImpl::getFifoFd() const
{
	return fifoFd;
}

bool const QWriteImpl::getWaitConnect() const
{
	return waitConnect;
}

} // namespace Ipc