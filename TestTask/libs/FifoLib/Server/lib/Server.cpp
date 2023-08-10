#include "Server/Server.h"
#include <iostream>

namespace Ipc {
void Server::getter(size_t id, FifoRead::Data&& data)
{
	readHandler(id, std::move(data));
};

void Server::connect(size_t id, const Fifo& object)
{
	std::lock_guard<std::mutex> mtx(mtxConnect);
	if(object.getWaitConnectWrite() && object.getWaitConnectRead()) {
		if(stateClient.at(id) == State::disconnect) {
			stateClient.at(id) = State::connect;
			std::cout << "Connect " << id << std::endl;
			connectHandler(id);
		}
	}
};

void Server::disconnect(size_t id, Fifo& object)
{
	std::lock_guard<std::mutex> mtx(mtxDisconnect);
	if(!object.getWaitConnectWrite() || !object.getWaitConnectRead()) {
		if(stateClient.at(id) == State::connect) {
			stateClient.at(id) = State::disconnect;
			std::cout << "Disconnect " << id << std::endl;
			disconnectHandler(id);
		}
	}
};

Server::Server(std::list<FifoCfg> const& nameChannelsfifo, Config config) :
    stateClient(nameChannelsfifo.size(), State::disconnect)
{
	size_t id = 0;
	for(auto const& name: nameChannelsfifo) {
		fifoCfgTable.insert({id, name});

		auto writer = WriterFactory::create(name.reverseFile, config);

		auto fifo = std::make_unique<Fifo>(std::move(writer), name.directFile);

		connectionTable.insert({id, std::move(fifo)});

		connectionTable[id]->setReadHandler([this, id](FifoRead::Data&& data) {
			this->getter(id, std::move(data));
		});

		connectionTable[id]->setConnectionHandlerRead([this, id]() {
			this->connect(id, *connectionTable[id]);
		});

		connectionTable[id]->setDisconnectionHandlerRead([this, id]() {
			connectionTable[id]->closeWrite();
			this->disconnect(id, *connectionTable[id]);
		});

		connectionTable[id]->setConnectionHandlerWrite([this, id]() {
			this->connect(id, *connectionTable[id]);
		});

		connectionTable[id]->setDisconnectionHandlerWrite([this, id]() {
			connectionTable[id]->closeRead();
			this->disconnect(id, *connectionTable[id]);
		});
		id++;
	}
	idCount = id;
}

void Server::start()
{
	if(!readHandler) {
		throw std::runtime_error("callback readHandler not set");
	}
	if(!connectHandler) {
		throw std::runtime_error("callback connectHandler not set");
	}
	if(!disconnectHandler) {
		throw std::runtime_error("callback disconnectHandler not set");
	}
	for(const auto& Fifo: connectionTable) {
		Fifo.second->start();
	}
}

void Server::stop()
{
	for(const auto& Fifo: connectionTable) {
		Fifo.second->stop();
	}
}

void Server::setReadHandler(ReadHandler h)
{
	readHandler = std::move(h);
}

void Server::setDisconnectHandler(EventHandler h)
{
	disconnectHandler = std::move(h);
}

void Server::setConnectHandler(EventHandler h)
{
	connectHandler = std::move(h);
}

void Server::writeId(size_t id, const void* data, size_t sizeInBytes)
{
	connectionTable[id]->write(data, sizeInBytes);
}
Server::~Server()
{
	for(int i = 0; i != idCount; i++) {
		unlink(fifoCfgTable[i].directFile.c_str());
		unlink(fifoCfgTable[i].reverseFile.c_str());
	}
}

void Server::stopId(size_t id)
{
	if(id < fifoCfgTable.size()) {
		connectionTable[id]->stop();
	}
	else
		throw std::runtime_error("no idClient");
}

void Server::startId(size_t id)
{
	if(id < fifoCfgTable.size()) {
		connectionTable[id]->start();
	}
	else
		throw std::runtime_error("no idClient");
}

std::unique_ptr<IFifoWriter> Server::WriterFactory::create(std::string filename, Config conf)
{
	switch(conf) {
	case(Config::QW):
		return std::make_unique<WriteQImpl>((filename));
	case(Config::NQW):
		return std::make_unique<WriteDirectImpl>((filename));
	default:
		throw std::runtime_error("no Config WriteFactory");
	}
}
} // namespace Ipc