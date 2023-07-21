#include "Fifo/Fifo.h"

// FifoException::FifoException(std::string const& msg) : runtime_error(msg.c_str())
//{}
//
// FifoWriteException::FifoWriteException(const std::string& msg) : FifoException(msg)
//{}
// FifoReadException::FifoReadException(const std::string& msg) : FifoException(msg)
//{}
// FifoAniException::FifoAniException(const std::string& msg) : FifoException(msg)

long FifoBase::openFifo(const std::string fdFileName, const char flag)
{
	long fd = -1;
	if(flag == 'W') {
		fd = open(fdFileName.c_str(), O_WRONLY, 0);
	}
	else if(flag == 'R') {
		fd = open(fdFileName.c_str(), O_RDONLY, 0);
	}
	if(-1 == fd)
		throw std::runtime_error(" fail openFifo ");
	return fd;
}

void FifoBase::createFifo(const std::string fdFileName)
{
	if((mkfifo(fdFileName.c_str(), FILE_MODE) < 0) && (errno != EEXIST)) {
		std::cout << fdFileName.c_str() << '\n';
		throw std::runtime_error(" fail createFifo ");
	}
}

FifoRead::FifoRead(const std::string fdFileName)
{
	params.addrRead = fdFileName;
	createFifo(params.addrRead);
}
void FifoRead::startRead()
{
	if(!params.msgHandler) {
		throw std::runtime_error("callback msgHandler not set");
	}
	if(!params.connectHandler) {
		throw std::runtime_error("callback connectHandler not set");
	}
	runRead               = true;
	threadWaitConnectFifo = std::make_unique<std::thread>(std::thread([this]() {
		waitConnectFifo();
	}));
}

void FifoRead::waitConnectFifo()
{
	fifoReadFd = openFifo(params.addrRead, 'R');
	if(runRead) {
		params.connectHandler(); // соединение проиошло
		waitConnect    = true;
		threadReadFifo = std::make_unique<std::thread>(std::thread([this]() {
			readFifo();
		}));
	}
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
	auto fd = openFifo(params.addrRead.c_str(), 'W');

	threadWaitConnectFifo->join();
	close(fd);
	close(fifoReadFd);
	if(waitConnect) {
		threadReadFifo->join();
	}
}
void FifoRead::setConnectionHandler(ConnectionHandler handler)
{
	params.connectHandler = std::move(handler);
}

void FifoRead::setReadHandler(FifoRead::ReadHandler handler)
{
	params.msgHandler = std::move(handler);
}

FifoRead::~FifoRead()
{
	unlink(params.addrRead.c_str());
}

FifoWrite::FifoWrite(std::string fdFileName) : fdFileName(fdFileName)
{
	createFifo(fdFileName);
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
	fifoFd = openFifo(fdFileName, 'W');
	if(runWrite) {
		// соединение проиошло
		waitConnect     = true;
		threadWriteFifo = std::make_unique<std::thread>(std::thread([this]() {
			writeFifo();
		}));
	}
}

void FifoWrite::stopWrite()
{
	runWrite = false;

	auto fd = open(fdFileName.c_str(), O_RDONLY, 0);

	threadWaitConnectFifo->join();
	close(fifoFd);
	close(fd);

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
				return;
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

Fifo::Fifo(const std::string fdFileNameWrite, const std::string fdFileNameRead) :
    fifoRead(fdFileNameRead), fifoWrite(fdFileNameWrite)
{}

void Fifo::setReadHandler(FifoRead::ReadHandler handler)
{
	fifoRead.setReadHandler(handler);
}

void Fifo::setConnectionHandler(FifoRead::ConnectionHandler handler)
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
void Server::getter(FifoRead::Data&& data)
{
	std::cout << "произошло событие" << std::endl;
	readHandler(connectionId, std::move(data));
};

void Server::connect()
{
	std::cout << "произошел коннект" << std::endl;
};
Server::Server(const std::vector<std::string>& nameChannelsfifo) : nameChannelsFifo(nameChannelsfifo)
{
	for(const auto& name: nameChannelsFifo) {
		connectionId[name] = std::make_unique<Fifo>(name, name + "_reverse");

		connectionId[name]->setReadHandler([this](FifoRead::Data&& data) {
			this->getter(std::move(data));
		});
		connectionId[name]->setConnectionHandler([this]() {
			this->connect();
		});
	}
}
void Server::start()
{
	if(!newHandler) {
		throw std::runtime_error("callback newHandler not set");
	}
	if(!closeHandler) {
		throw std::runtime_error("callback closeHandler not set");
	}
	if(!readHandler) {
		throw std::runtime_error("callback readHandler not set");
	}

	for(const auto& Fifo: connectionId) {
		Fifo.second->start();
	}
}
void Server::stop()
{
	for(const auto& Fifo: connectionId) {
		Fifo.second->stop();
	}
}
void Server::setReadHandler(ReadHandler h)
{
	readHandler = std::move(h);
}
void Server::setNewConnectionHandler(Server::ConnChangeHandler h)
{
	newHandler = std::move(h);
}

void Server::setCloseConnectionHandler(Server::ConnChangeHandler h)
{
	closeHandler = std::move(h);
}
