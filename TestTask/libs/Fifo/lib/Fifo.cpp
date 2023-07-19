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
	FIFO = params.addrRead.c_str();
	createFifo();
}
void FifoRead::createFifo()
{
	if((mkfifo(FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << FIFO << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}

uint8_t FifoRead::openFifoRead()
{
	uint8_t fd = open(FIFO, O_RDONLY, 0);
	if(-1 == fd) {
		throw std::runtime_error("fail openFifoRead");
	}
	return fd;
}

void FifoRead::startRead()
{
	runRead        = true;
	threadWaitConnectFifo = std::make_unique<std::thread>(std::thread([this]() {
		waitConnectFifo();
	}));

}

void FifoRead::stopRead()
{
	runRead = false;
	close(fifoFd);
}

FifoWrite::FifoWrite(const std::string& fdFileName) : FIFO(fdFileName.c_str())
{
	createFifo();
}
uint8_t FifoWrite::openFifoWrite()
{
	uint8_t fd = open(FIFO, O_WRONLY, 0);
	if(-1 == fd) {
		throw std::runtime_error("fail openFifoWrite");
	}
	return fd;
}

void FifoWrite::createFifo()
{
	if((mkfifo(FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << FIFO << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}

void FifoWrite::startWrite()
{
	runWrite        = true;
	threadWriteFifo = std::make_unique<std::thread>(std::thread([this]() {
		writeFifo();
	}));

}

void FifoWrite::stopWrite()
{
	runWrite = false;
	close(fifoFd);
	threadWriteFifo->join();
}

void FifoWrite::writeFifo()
{
	fifoFd = openFifoWrite(); // должны находиться здесь
	std::cout << "\n Произошел коннект \n";

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


void FifoRead::waitConnectFifo()
{
	fifoFd = openFifoRead();
    params.connectHandler();//соединение проиошло
	waitConnect= true;
	threadReadFifo = std::make_unique<std::thread>(std::thread([this]() {
	readFifo();
	}));
}



FifoRead::~FifoRead()
{
unlink(FIFO);
threadWaitConnectFifo->detach();
if(waitConnect) {
		threadReadFifo->join();
}
}


void FifoRead::readFifo()
{
std::vector<uint8_t> buffer(1024*8);

while(runRead) {
		auto flag = read(fifoFd, buffer.data(), 1024 * 8);
		if(flag == 0) {
		break;
		}
		if(flag == 1024) {
		params.msgHandler(std::move(buffer));
		}
		else
		params.msgHandler(std::vector<uint8_t>(buffer.data(), buffer.data() + flag));
}
}
