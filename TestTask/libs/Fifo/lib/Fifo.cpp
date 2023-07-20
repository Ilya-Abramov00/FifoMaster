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
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/signal.h>
FifoRead::FifoRead(const std::string fdFileName)
{
	params.addrRead = fdFileName;
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
	if(!params.msgHandler) {
		throw std::runtime_error("callback for msg getting not set");
	}
	if(!params.connectHandler) {
		throw std::runtime_error("callback for msg getting not set");
	}
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
}

void FifoRead::readFifo()
{
	auto MAXLINE = 1024 * 64;
	std::vector<uint8_t> buffer(MAXLINE);

	while(runRead) {
		auto flag = read(fifoReadFd, buffer.data(), MAXLINE);
		if(flag == 0) {
			break;
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
	close(fifoReadFd);
	threadWaitConnectFifo->detach();
	if(waitConnect) {
		threadReadFifo->join();
	}
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
		std::lock_guard<std::mutex> mtx_0(mtx);
		if(!queue.empty()) {
			signal(SIGPIPE, SIG_IGN); // отлавливает сигнал в случае закрытия канала на чтение
			auto flag = write(fifoFd, queue.front().data(), queue.front().size());
			if(flag == -1) {
				runWrite = false;
				std::cerr << "\n принимающая сторона закрыла канал \n";
			}
			queue.pop();
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
	std::lock_guard<std::mutex> mtx_0(mtx);
	queue.push(std::move(buffer));
}
FifoWrite::~FifoWrite()
{}

Fifo::Fifo(const std::string fdFileName) : FifoRead(fdFileName), FifoWrite(fdFileName)
{}
