#include "Fifo/Fifo.h"

FifoException::FifoException(std::string const& msg) : runtime_error(msg.c_str())
{}

FifoWriteException::FifoWriteException(const std::string& msg) : FifoException(msg)
{}
FifoReadException::FifoReadException(const std::string& msg) : FifoException(msg)
{}
FifoAniException::FifoAniException(const std::string& msg) : FifoException(msg)
{}

void FifoRead::createFifo(const char* FIFO)
{
	if((mkfifo(FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << FIFO << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}

void FifoRead::startRead()
{
	run_read = true;
	readFifo();
}

void FifoRead::stopRead()
{
	run_read = false;
}

void FifoRead::readFifo()
{
	std::vector<char> read_buffer(params.dataUnitSize);

	uint8_t fifo_fd = openFifoRead(FIFO);

	std::string buf = "";
	long flag       = 0;
	while(run_read) {
		flag += readFifo(fifo_fd, read_buffer.data() + flag, params.dataUnitSize - flag);
		if(flag == 0) {
			break;
		}
		if(flag == params.dataUnitSize) {
			params.msgHandler(read_buffer.data(), params.dataUnitSize);
			flag = 0;
			read_buffer.clear();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(params.timeToWaitDataNanoSeconds));
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

long FifoRead::readFifo(uint8_t fifo_fd, char* read_buffer, size_t N)
{
	memset(read_buffer, 0, N);

	auto flag = read(fifo_fd, read_buffer, N);

	return flag;
}
FifoRead::FifoRead(Params& params) : params(params)
{
	if(params.dataUnitSize > 1024 * 64)
		throw std::runtime_error("fail very big size_N ");
	FIFO = params.fdFileName.c_str();
	createFifo(FIFO);
	// можно попробовать убрать}
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
	writeUser();
}

void FifoWrite::stopWrite()
{
	run_write = false;
}

void FifoWrite::writeFifo()
{
	int8_t fifo_write_fd = openFifoWrite(FIFO);
	while(run_write) {
		writeFifo(fifo_write_fd, getmsg().c_str());
	}
	close(fifo_write_fd);
	unlink(FIFO);
}

int FifoWrite::openFifoWrite(const char* FIFO)
{
	int fd = open(FIFO, O_WRONLY, 0);
	if(-1 == fd) {
		throw std::runtime_error("fail openFifoWrite");
	}
	return fd;
}

void FifoWrite::writeFifo(int fifo_fd, const char* data)
{
	if(fifo_fd == -1) {
		throw std::runtime_error("fail writeFifo");
	}
	write(fifo_fd, data, strlen(data));
}

void FifoWrite::createFifo(const char* FIFO)
{
	if((mkfifo(FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << FIFO << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}
FifoWrite::FifoWrite(std::string& fdFileName, std::mutex& mtx) : FIFO(fdFileName.c_str()), mtx(mtx)
{
	createFifo(FIFO);
}

void FifoWrite::writeUser()
{
	auto e = getmsg();

	auto ptr = reinterpret_cast<uint8_t*>(&e);

	auto buffer = std::vector<uint8_t>(ptr, ptr + size(e));
}