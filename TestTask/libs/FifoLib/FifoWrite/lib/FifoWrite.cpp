#include "FifoWrite/FifoWrite.h"



#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <mutex>
#include <queue>
#include <sys/signal.h>


FifoWriteQueue::FifoWriteQueue(std::string fdFileName)
{
	params.addrRead = fdFileName;
	createFifo(params.addrRead);
}

void FifoWriteQueue::setConnectionHandler(FifoBase::ConnectionHandler handler)
{
	params.connectHandler = std::move(handler);
}

void FifoWriteQueue::setDisConnectionHandler(FifoBase::ConnectionHandler handler)
{
	params.disconnectHandler = std::move(handler);
}

void FifoWriteQueue::startWrite()
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
void FifoWriteQueue::waitConnectFifo()
{
	fifoFd = openFifo(params.addrRead, 'W');
	if(runWrite) {
		waitConnect     = true;
		params.connectHandler();
		threadWriteFifo = std::make_unique<std::thread>(std::thread([this]() {
			writeFifo();
		}));
	}
}

void FifoWriteQueue::stopWrite()
{
		runWrite = false;
		if(!waitConnect) {
			auto fd = openFifo(params.addrRead.c_str(), 'R');
			close(fd);
		}

		threadWaitConnectFifo->join();
		close(fifoFd);

		if(waitConnect) {
			threadWriteFifo->join();
		}
		if(queue.size()) {
			std::cerr <<getName()<< " канал не смог отправить данные в полном размере";
		}
}

void FifoWriteQueue::writeFifo()
{
	if(fifoFd == -1) {
		throw std::runtime_error(" fail openFifo ");
	}
	while(runWrite) {
		{
			std::lock_guard<std::mutex> mtx_0(mtx);
			if(!queue.empty()) {
				signal(SIGPIPE, SIG_IGN); // отлавливает сигнал в случае закрытия канала на чтение
				auto flag = write(fifoFd, queue.front().data(), queue.front().size());
				if(flag == -1) {
					waitDisConnect = true;
					params.disconnectHandler();
					return;
				}
				queue.pop();
			}
		}
	}
}

void FifoWriteQueue::pushData(const void* data, size_t sizeN)
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
bool const FifoWriteQueue::getWaitDisconnect() const
{
	return waitDisConnect;
}
std::string const FifoWriteQueue::getName() const
{
	return params.addrRead;
}
long const& FifoWriteQueue::getFifoFd() const
{
	return fifoFd;
}
bool const FifoWriteQueue::getWaitConnect() const
{
	return waitConnect;
}


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
