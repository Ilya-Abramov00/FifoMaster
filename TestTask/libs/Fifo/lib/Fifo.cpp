#include "Fifo/Fifo.h"

// FifoException::FifoException(std::string const& msg) : runtime_error(msg.c_str())
//{}
//
// FifoWriteException::FifoWriteException(const std::string& msg) : FifoException(msg)
//{}
// FifoReadException::FifoReadException(const std::string& msg) : FifoException(msg)
//{}
// FifoAniException::FifoAniException(const std::string& msg) : FifoException(msg)
//{}

FifoRead::FifoRead(const Params& params) : params(params)
{
	createFifo();
}
void FifoRead::createFifo()
{
	if((mkfifo(params.addrRead.c_str(), FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << params.addrRead.c_str() << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}

long FifoRead::openFifoRead()
{
	long fd = open(params.addrRead.c_str(), O_RDONLY, 0);
	if(-1 == fd) {
		throw std::runtime_error("fail openFifoRead");
	}
	return fd;
}

void FifoRead::startRead()
{
	runRead               = true;
	threadWaitConnectFifo = std::make_unique<std::thread>(std::thread([this]() {
		waitConnectFifo();
	}));
}
void FifoRead::waitConnectFifo()
{
	fifoReadFd = openFifoRead();
	params.connectHandler(); // соединение проиошло
	waitConnect    = true;
	threadReadFifo = std::make_unique<std::thread>(std::thread([this]() {
		readFifo();
	}));
}

FifoRead::~FifoRead()
{
	unlink(params.addrRead.c_str());
	threadWaitConnectFifo->detach();
	if(waitConnect) {
		threadReadFifo->join();
	}
}

void FifoRead::readFifo()
{
	std::vector<uint8_t> buffer(1024);
	while(runRead) {
		auto flag = read(fifoReadFd, buffer.data(), 1024);
		if(flag == 0) {
			break;
		}
		if(flag == 1024) {
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
	close(fifoReadFd);
}

FifoWrite::FifoWrite(std::string fdFileName) : fdFileName(fdFileName)
{
	createFifo();
}
long FifoWrite::openFifoWrite()
{
	long fd = open(fdFileName.c_str(), O_WRONLY, 0);
	if(-1 == fd) {
		throw std::runtime_error("fail openFifoWrite");
	}
	return fd;
}

void FifoWrite::createFifo()
{
	if((mkfifo(fdFileName.c_str(), FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << fdFileName.c_str() << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}

void FifoWrite::startWrite()
{
	runWrite = true;

	threadWaitConnectFifo = std::make_unique<std::thread>(std::thread([this]() {
		waitConnectFifo();
	}));
}

void FifoWrite::waitConnectFifo()
{
	fifoFd = openFifoWrite();
	std::cout << "\n Произошел коннект \n";
	// соединение проиошло

	waitConnect     = true;
	threadWriteFifo = std::make_unique<std::thread>(std::thread([this]() {
		writeFifo();
	}));
}
void FifoWrite::stopWrite()
{
	runWrite = false;
	close(fifoFd);
	threadWaitConnectFifo->detach();

	if(waitConnect) {
		threadWriteFifo->join();
	}
}

void FifoWrite::writeFifo()
{
	while(runWrite) {
		{
			std::lock_guard<std::mutex> mtx_0(mtx);
			if(!queue.empty()) {
				write(fifoFd, queue.front().data(), queue.front().size());
				queue.pop();
			}
		}
	}
}

void FifoWrite::pushData(void* data, size_t sizeN)
{
	if(!data) {
		std::cerr << "\n null ptr is pushData \n";
		return;
	}
	auto ptr = reinterpret_cast<uint8_t*>(data);
	std::vector<uint8_t> buffer(ptr, ptr + sizeN);
	{
		std::lock_guard<std::mutex> mtx_0(mtx);
		queue.push(std::move(buffer));
	}
}
FifoWrite::~FifoWrite()
{}

Fifo::Fifo(Params params) : FifoRead(params), FifoWrite(params.addrRead)
{}
