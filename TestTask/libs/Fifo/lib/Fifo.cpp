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

// flag += readFifo(fifoFd, read_buffer.data() + flag, params.dataUnitSize - flag);
// if(flag == 0) {
//	break;
// }
// if(flag == params.dataUnitSize) {
//	params.msgHandler(read_buffer.data(), params.dataUnitSize);
//	flag = 0;
//	read_buffer.clear();
// }
// std::this_thread::sleep_for(std::chrono::milliseconds(params.timeToWaitDataNanoSeconds));
// }
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
		std::pair<void*, size_t> temporaryBuffer = getmsg();

		auto ptr = reinterpret_cast<uint8_t*>(temporaryBuffer.first);

		auto buffer = std::vector<uint8_t>(ptr, ptr + temporaryBuffer.second);

		// std::unique_lock<std::mutex> mtx_0(mtx);
		//	mtx_0.lock();
		if(queue.empty()) {
			write(fifoFd, buffer.data(), temporaryBuffer.second);
		}
		else {
			// queue.push(std::move(buffer));
		}
		//	mtx_0.unlock();
	}
	close(fifoFd);
	//});
}

void FifoRead::readFifo()
{
	uint8_t fifoFd    = openFifoRead(FIFO);
	char* read_buffer = new char[params.dataUnitSize];
	long flag         = 0;
	int x             = 0;
	while(run_read) {
		flag = read(fifoFd, read_buffer, params.dataUnitSize - x);
		if(flag == 0) {
			break;
		}
		params.msgHandler(read_buffer, flag);
		memset(read_buffer, 0, flag);
		std::this_thread::sleep_for(std::chrono::milliseconds(params.timeToWaitDataNanoSeconds));
	}
	delete[] read_buffer;
}