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

FifoRead::FifoRead(Params& params) : params(params)
{
	if(params.dataUnitSize > 1024 * 64)
		throw std::runtime_error("fail very big size_N ");
	FIFO = params.fdFileName.c_str();
	createFifo(FIFO);
}
void FifoRead::createFifo(const char* FIFO)
{
	if((mkfifo(FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << FIFO << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}

int FifoRead::openFifoRead(const char* FIFO)
{
	int fd = open(FIFO, O_RDONLY, 0);
	if(-1 == fd) {
		throw std::runtime_error("fail openFifoRead");
	}
	return fd;
}

void FifoRead::startRead()
{
	run_read = true;

	// threadReadFifo = std::thread([this]() {
	readFifo();
	//	});
}

void FifoRead::stopRead()
{
	run_read = false;
	// threadReadFifo.join();
}

FifoWrite::FifoWrite(std::string& fdFileName) : FIFO(fdFileName.c_str())
{
	createFifo(FIFO);
}
int FifoWrite::openFifoWrite(const char* FIFO)
{
	int fd = open(FIFO, O_WRONLY, 0);
	if(-1 == fd) {
		throw std::runtime_error("fail openFifoWrite");
	}
	return fd;
}

void FifoWrite::createFifo(const char* FIFO)
{
	if((mkfifo(FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << FIFO << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}

void FifoWrite::setMsgGetter(FifoWrite::MsgGetter msgGetter)
{
	getmsg = std::move(msgGetter);
}

void FifoWrite::startWrite()
{
	if(!getmsg) {
		throw std::runtime_error("callback for msg getting not set");
	}
	run_write = true;
	// threadWriteFifo = std::thread([this]() {
	//	writeFifo();
	// });
}

void FifoWrite::stopWrite()
{
	run_write = false;
	// threadWriteFifo.join();
	// threadUserWrite.join();
}

void FifoWrite::writeFifo()
{
	while(run_write) {
		// std::unique_lock<std::mutex> mtx_0(mtx);
		// if(!queue.empty()) {
		// std::cout << "данные записались" << queue.front().data();
		write(fifoFd, queue.front().data(), queue.front().size());
		queue.pop();
		//}
	}
	close(fifoFd);
	unlink(FIFO);
}

void FifoWrite::writeUser()
{
	// threadUserWrite = std::thread([this]() {
	fifoFd = openFifoWrite(FIFO);
	while(run_write) {
		std::pair<std::string, size_t> temporaryBuffer = getmsg();

		auto ptr = (temporaryBuffer.first.c_str());

		auto buffer = std::vector<char>(ptr, ptr + temporaryBuffer.second);

		for(int i = 0; i != buffer.size(); i++) {
			if(temporaryBuffer.first[i] != buffer[i]) {
				throw 3;
			}
		}
		// std::unique_lock<std::mutex> mtx_0(mtx);
		//	mtx_0.lock();
		if(queue.empty()) {
			write(fifoFd, buffer.data(), buffer.size());
		}
		else {
			// queue.push(std::move(buffer));
		}
		//	mtx_0.unlock();
	}
	//});
}
void FifoRead::readFifo()
{
	uint8_t fifoFd = openFifoRead(FIFO);

	char* read_buffer = new char[params.dataUnitSize];
	long flag         = 0;
	std::string bufBuf = "";
	while(run_read) {
		flag += read(fifoFd, read_buffer, params.dataUnitSize - flag);
		if(flag == 0) {
			break;
		}
		bufBuf += read_buffer;
		if(flag == params.dataUnitSize) {
			params.msgHandler(bufBuf, params.dataUnitSize);
			flag = 0;
			memset(read_buffer, 0, params.dataUnitSize);

			bufBuf.clear();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(params.timeToWaitDataNanoSeconds));
	}
	if(!bufBuf.empty()){params.msgHandler(bufBuf, params.dataUnitSize);}

	delete[] read_buffer;
}
