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
	if(params.dataUnitSize > 1024 * 64)
		throw std::runtime_error("fail very big size_N ");
	FIFO = params.fdFileName.c_str();
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
	runRead = true;
	readFifo();
}

void FifoRead::stopRead()
{
	runRead = false;
	threadReadFifo->join();
	close(fifoFd);
	unlink(FIFO);
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
	runWrite = true;
	writeFifo();
}

void FifoWrite::stopWrite()
{
	runWrite = false;
	threadWriteFifo->join();
	close(fifoFd);
	unlink(FIFO);
}

void FifoWrite::writeFifo()
{
	threadWriteFifo = std::make_unique<std::thread>(std::thread([this]() {
		fifoFd = openFifoWrite(); // должны находиться здесь
		while(runWrite) {
			std::lock_guard<std::mutex> mtx_0(mtx);
			if(!queue.empty()) {
				write(fifoFd, queue.front().data(), queue.front().size());
				queue.pop();
			}
		}
	}));
}

void FifoWrite::writeUser(void* data, size_t sizeN)
{
	if(!data) {
		std::cerr << "\n null ptr is writeUser \n";
	}
else {
		auto ptr    = reinterpret_cast<uint8_t*>(data);
		int Maxline = 1024 * 64;

		for(size_t i = 0; sizeN > Maxline * i; i++) {
			std::vector<uint8_t> buffer(ptr, ptr + sizeN - i * Maxline);
			std::lock_guard<std::mutex> mtx_0(mtx);
			queue.push(std::move(buffer)); // это работать не будет, переделать(цикл)
		}
	}
}

void FifoRead::readFifo()
{
	threadReadFifo = std::make_unique<std::thread>(std::thread([this]() {
		fifoFd       = openFifoRead(); // должны находиться здесь
		auto Maxline = 64 * 1024;
		std::vector<uint8_t> read_buffer(Maxline);

		while(runRead) {
			auto flag = read(fifoFd, read_buffer.data(), Maxline);
			if(flag == 0) {
				break;
			}
			params.msgHandler(read_buffer.data(), flag);
			read_buffer.clear();
		}
	}));
}
