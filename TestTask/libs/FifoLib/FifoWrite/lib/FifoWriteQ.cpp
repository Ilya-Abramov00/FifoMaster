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

QWriteImpl::QWriteImpl(std::string fdFileName) : params{fdFileName}
{
	createFifo(params.addrRead);
}

void QWriteImpl::setConnectionHandler(ConnectionHandler handler)
{
	params.connectHandler = std::move(handler);
}

void QWriteImpl::setDisConnectionHandler(ConnectionHandler handler)
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
	runWrite        = true;
	threadWriteFifo = std::make_unique<std::thread>([this]() {
		writeFifo();
	});
}

void QWriteImpl::writeFifo()
{
	while(runWrite) {
		connect();
		while(waitConnect && runWrite && (fifoFd != -1)) {
			{
				std::lock_guard<std::mutex> mtx_0(mtx);
				if(!queue.empty()) {
					signal(SIGPIPE, SIG_IGN); // отлавливает сигнал в случае закрытия канала на чтение
					auto flag = write(fifoFd, queue.front().data(), queue.front().size());
					if(flag == -1) {
						waitConnect= false;
						params.disconnectHandler();

						break;
					}
					queue.pop();
				}
			}
		}
	}
}

void QWriteImpl::connect()
{
	waitOpen = false;
	fifoFd   = openFifo(params.addrRead, 'W');
	waitOpen = true;

	if(runWrite && (fifoFd != -1)) {
		waitConnect= true;
		params.connectHandler();
	};
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
		queue.emplace(std::move(buffer));
	}
}

void QWriteImpl::stopWrite()
{
	runWrite    = false;

	params.disconnectHandler();
	if(!waitOpen) {
		auto fd = openFifo(params.addrRead.c_str(), 'R');
		close(fd);
	}
	close(fifoFd);

	threadWriteFifo->join();
	waitConnect= false;
	if(queue.size()) {
		std::cerr << params.addrRead << " в очереди остались неотправленные сообщения\n";
	}
}

bool  QWriteImpl::getWaitConnect() const
{
	return waitConnect;
}

long const& QWriteImpl::getFifoFd() const
{
	return fifoFd;
}



} // namespace Ipc