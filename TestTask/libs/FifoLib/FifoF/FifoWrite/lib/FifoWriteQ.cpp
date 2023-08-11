#include "FifoWrite/FifoWriteQ.h"

#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <sys/signal.h>

namespace Ipc {

WriteQImpl::WriteQImpl(std::string fdFileName) : params{fdFileName}
{
	createFifo(params.addrRead);
}

void WriteQImpl::setConnectionHandler(ConnectionHandler handler)
{
	params.connectHandler = std::move(handler);
}

void WriteQImpl::setDisconnectionHandler(ConnectionHandler handler)
{
	params.disconnectHandler = std::move(handler);
}

void WriteQImpl::startWrite()
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

void WriteQImpl::writeFifo()
{
	while(runWrite) {
		connect();
		while(waitConnect && runWrite) {
			{
				std::lock_guard<std::mutex> mtx_0(mtx);
				if(!queue.empty()) {
					signal(SIGPIPE, SIG_IGN); // отлавливает сигнал в случае закрытия канала на чтение
					auto flag = write(fifoFd, queue.front().data(), queue.front().size());
					if(flag == -1) {
						waitConnect = false;
						params.disconnectHandler();

						break;
					}
					queue.pop();
				}
			}
		}
	}
}

void WriteQImpl::connect()
{
	waitOpen = false;
	fifoFd   = openFifo(params.addrRead, 'W');
	waitOpen = true;

	if(runWrite && (fifoFd != -1)) {
		waitConnect = true;
		params.connectHandler();
	};
}
void WriteQImpl::pushData(const void* data, size_t sizeN)
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

void WriteQImpl::stopWrite()
{
	runWrite = false;

	params.disconnectHandler();
	if(!waitOpen) {
		auto fd = openFifo(params.addrRead.c_str(), 'R');
		close(fd);
	}
	close(fifoFd);

	threadWriteFifo->join();
	waitConnect = false;
	if(queue.size()) {
		std::cerr << params.addrRead << " в очереди остались неотправленные сообщения\n";
	}
}

bool WriteQImpl::getWaitConnect() const
{
	return waitConnect;
}

long const& WriteQImpl::getFifoFd() const
{
	return fifoFd;
}

} // namespace Ipc