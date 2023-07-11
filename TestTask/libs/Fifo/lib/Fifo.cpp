#include "Fifo/Fifo.h"

void FifoRead::createFifo(const char* FIFO)
{
	if((mkfifo(FIFO, FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << FIFO << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}
FifoRead::FifoRead(std::string& Fifo_write_q)
{
	FIFO = Fifo_write_q.c_str();
	createFifo(FIFO);
}

void FifoRead::start_read()
{
	run_read = true;
}

void FifoRead::stop_read()
{
	run_read = false;
}

void FifoRead::readFifo(std::string& data, size_t N)
{

	char* read_buffer = new char[N];

	if(N > 1024 * 64)
		throw std::runtime_error("fail very big N ");

	while(run_read) {
		readFifo(FIFO, read_buffer, N);
		data.append(read_buffer);
	}

	delete read_buffer;
}

int FifoRead::openFifoRead(const char* FIFO)
{
	int fd = open(FIFO, O_RDONLY, 0);
	if(-1 == fd) {
		throw std::runtime_error("fail openFifoWrite");
	}

	return fd;
}

void FifoRead::readFifo(const char* FIFO, char* read_buffer, size_t N)
{
	uint8_t fifo_fd = openFifoRead(FIFO);
	memset(read_buffer, 0, N);
	read(fifo_fd, read_buffer, N);
}

void FifoWrite::setMsgGetter(FifoWrite::MsgGetter msgGetter)
{
	getmsg = std::move(msgGetter);
}

void FifoWrite::start_write()
{
	if(!getmsg) {
		throw std::runtime_error("callback for msg getting not set");
	}
	run_write = true;
}

void FifoWrite::stop_write()
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
FifoWrite::FifoWrite(std::string& Fifo_write)
{
	FIFO = Fifo_write.c_str();
	createFifo(FIFO);
}
