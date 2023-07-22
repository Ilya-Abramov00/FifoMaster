#include "FifoWrite/FifoWrite.h"

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
		params.connectHandler();
		waitConnect     = true;
		threadWriteFifo = std::make_unique<std::thread>(std::thread([this]() {
			writeFifo();
		}));
	}
}

void FifoWrite::stopWrite()
{
	runWrite = false;

	auto fd        = open(params.addrRead.c_str(), O_RDONLY, 0);
	waitDisConnect = false;
	threadWaitConnectFifo->join();
	close(fifoFd);
	close(fd);

	if(waitConnect) {
		threadWriteFifo->join();
	}
	if(queue.size()){std::cerr<<"данные не отправились\n";}
}

void FifoWrite::writeFifo()
{
	if(fifoFd == -1) {
		throw std::runtime_error(" fail openFifo ");
	}
	while(runWrite) {
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

void FifoWrite::pushData(const void* data, size_t sizeN)
{
	if(!data) {
		std::cerr << "\n null ptr is pushData \n";
		return;
	}
	auto ptr = reinterpret_cast<const uint8_t*>(data);
	std::vector<uint8_t> buffer(ptr, ptr + sizeN);
	std::lock_guard<std::mutex> mtx_0(mtx);
	queue.push(std::move(buffer));
}
