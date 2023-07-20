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



void FifoRead::readFifo()
{
	auto MAXLINE = 1024 * 64;
	std::vector<uint8_t> buffer(MAXLINE);

	while(runRead) {
		auto flag = read(fifoReadFd, buffer.data(), MAXLINE);
		if(flag == 0) {
			return;
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
	threadWaitConnectFifo->detach();
	if(waitConnect) {
		threadReadFifo->join();
		close(fifoReadFd);
		unlink(params.addrRead.c_str());
	}
}
void FifoRead::setConnectionHandler(ConnectionHandler handler)
{
	params.connectHandler = std::move(handler);
}
void FifoRead::setReadHandler(ReadsHandler handler)
{
	params.msgHandler = std::move(handler);
}
const bool FifoRead::getBooLWaitConnect() const
{
	return waitConnect;
}

FifoWrite::FifoWrite(std::string fdFileName) : fdFileName(fdFileName)
{
	createFifo();
}
long FifoWrite::openFifoWrite()
{
	long fd = open(fdFileName.c_str(), O_WRONLY, 0);

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

	threadWaitConnectFifo->detach();

	if(waitConnect) {
		threadWriteFifo->join();
		close(fifoFd);
		unlink(fdFileName.c_str()); // хз нужно ли
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
				return;
				std::cerr << "\n принимающая сторона закрыла канал \n";
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

const bool FifoWrite::getWaitConnect() const
{
	return waitConnect;
}


Fifo::Fifo(const std::string fdFileNameWrite, const std::string fdFileNameRead) :
    fifoRead(fdFileNameRead), fifoWrite(fdFileNameWrite)
{}

void Fifo::setReadHandler(ReadsHandler handler)
{
	fifoRead.setReadHandler(handler);
}

void Fifo::setConnectionHandler(ConnectionHandler handler)
{
	fifoRead.setConnectionHandler(handler);
}

void Fifo::write(const void* data, size_t sizeInBytes)
{
	fifoWrite.pushData(data, sizeInBytes);
}
void Fifo::stop()
{
	fifoWrite.stopWrite();
	fifoRead.stopRead();
}
void Fifo::start()
{
	fifoRead.startRead();
	fifoWrite.startWrite();
}

auto getterRead = [](Data&& dataq) {
	std::cout << "произошло событие" << std::endl;
};
auto connect = []() {
	std::cout << "произошел коннект" << std::endl;
};

Server::Server(const std::vector<std::string>& nameChannelsfifo) : nameChannelsFifo(nameChannelsfifo)
{
	for(const auto& name: nameChannelsFifo) {
		connectionId[name] = std::make_unique<Fifo>(name, name + "_reverse");
		connectionId[name]->setReadHandler(getterRead);
		connectionId[name]->setConnectionHandler(connect);
	}
}
