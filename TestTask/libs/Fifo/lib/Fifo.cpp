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

uint8_t FifoRead::openFifoRead(const char* FIFO)
{
	uint8_t fd = open(FIFO, O_RDONLY, 0);
	if(-1 == fd) {
		throw std::runtime_error("fail openFifoRead");
	}
	return fd;
}

void FifoRead::startRead()
{
	run_read       = true;
	threadReadFifo = std::move(std::unique_ptr<std::thread>(new std::thread([this]() {
		readFifo();
	})));
}

void FifoRead::stopRead()
{
	run_read = false;
	threadReadFifo->join();
}

FifoWrite::FifoWrite(std::string& fdFileName,std::mutex& mtx ) : FIFO(fdFileName.c_str()), mtx(mtx)
{
	createFifo(FIFO);
}
uint8_t FifoWrite::openFifoWrite(const char* FIFO)
{
	uint8_t fd = open(FIFO, O_WRONLY, 0);
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
	run_write       = true;
	threadUserWrite = std::move(std::unique_ptr<std::thread>(new std::thread([this]() {
		writeUser();
	})));
}

void FifoWrite::stopWrite()
{
	run_write = false;
	// threadWriteFifo.join();
	threadUserWrite->join();
}

void FifoWrite::writeFifo()
{
	while(run_write) {
		std::unique_lock<std::mutex> mtx_0(mtx);
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
	fifoFd = openFifoWrite(FIFO);
	while(run_write) {
		std::pair<void*, size_t> temporaryBuffer = getmsg();

		auto ptr = reinterpret_cast<uint8_t*>(temporaryBuffer.first);

		auto buffer = std::vector<uint8_t>(ptr, ptr + temporaryBuffer.second);

		 std::unique_lock<std::mutex> mtx_0(mtx);

		if(queue.empty()) {
			write(fifoFd, buffer.data(), temporaryBuffer.second);
		}
		else {
			// queue.push(std::move(buffer));
		}
			mtx_0.unlock();
	}
	close(fifoFd);
}

void FifoRead::readFifo()
{
	fifoFd = openFifoRead(FIFO);
	std::vector<uint8_t> read_buffer;

	long flag  = 0;
	long flagN = 0;
	while(run_read) {
		std::vector<uint8_t> timeRead_buffer(params.dataUnitSize - flagN);

		flag = read(fifoFd, timeRead_buffer.data(), params.dataUnitSize - flagN);
		if(flag == 0) {
			break;
		}
		flagN += flag;
		read_buffer.insert(read_buffer.begin(), timeRead_buffer.begin(), timeRead_buffer.end());
		if(flagN == params.dataUnitSize) {
			params.msgHandler(read_buffer.data(), params.dataUnitSize);
			flagN = 0;
			read_buffer.clear();
		}
		std::this_thread::sleep_for(std::chrono::nanoseconds(params.timeToWaitDataNanoSeconds));
	}
	if(read_buffer.size()) {
		params.msgHandler(read_buffer.data(), flagN);
	}
}

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