#include "Server/Server.h"
#include <iostream>

namespace Ipc {
void Server::getter(size_t id, FifoRead::Data&& data)
{
	readHandler(id, std::move(data));
};

void Server::connectH(ConnectionId id, const Fifo& object)
{
	if(object.getWaitConnectWrite() && object.getWaitConnectRead()) {
		if(stateClient.at(id) == State::disconnect) {
			stateClient.at(id) = State::connect;
			std::cout << "Connect " << id << std::endl;
			newHandler(id);
		}
	}
};

void Server::disconnectH(ConnectionId id, Fifo& object)
{
	if(!object.getWaitConnectWrite() || !object.getWaitConnectRead()) {
		if(stateClient.at(id) == State::connect) {
			stateClient.at(id) = State::disconnect;
			std::cout << "Disconnect " << id << std::endl;
			closeHandler(id);
		}
	}
};

Server::Server(std::list<FifoCfg> const& nameChannelsFifo, Config config,
               std::optional<size_t> waitConnectTimeMilliSeconds, std::optional<size_t> waitReconnectTimeMilliSeconds) :
    stateClient(nameChannelsFifo.size(), State::disconnect)
{
	size_t id = 0;
	for(auto const& name: nameChannelsFifo) {
		fifoCfgTable.insert({id, name});

		auto writer = WriterFactory::create(name.reverseFile, config, waitConnectTimeMilliSeconds.value(),
		                                    waitReconnectTimeMilliSeconds.value());

		auto fifo = std::make_unique<Fifo>(std::move(writer), name.directFile);

		connectionTable.insert({id, std::move(fifo)});

		connectionTable[id]->recoonectTrue();

		connectionTable[id]->setReadHandler([this, id](FifoRead::Data&& data) {
			this->getter(id, std::move(data));
		});

		connectionTable[id]->setConnectionHandlerRead([this, id]() {
			this->connectH(id, *connectionTable[id]);
		});

		connectionTable[id]->setConnectionHandlerWrite([this, id]() {
			this->connectH(id, *connectionTable[id]);
		});

		connectionTable[id]->setDisconnectionHandlerRead([this, id]() {
			connectionTable[id]->closeWrite();
			this->disconnectH(id, *connectionTable[id]);
		});

		connectionTable[id]->setDisconnectionHandlerWrite([this, id]() {
			this->disconnectH(id, *connectionTable[id]);
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
	if(!newHandler) {
		throw std::runtime_error("callback newHandler not set");
	}
	if(!closeHandler) {
		throw std::runtime_error("callback closeHandler not set");
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

void Server::setDisconnectHandler(ConnChangeHandler h)
{
	closeHandler = std::move(h);
}

void Server::setConnectHandler(ConnChangeHandler h)
{
	newHandler = std::move(h);
}

void Server::write(size_t id, const void* data, size_t sizeInBytes)
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

void Server::disconnect(size_t id)
{
	if(id < fifoCfgTable.size()) {
		connectionTable[id]->stop();
	}
	else
		throw std::runtime_error("no idClient");
}

std::unique_ptr<IFifoWriter> Server::WriterFactory::create(std::string filename, Config conf,
                                                           size_t waitConnectTimeMilliSeconds,
                                                           size_t waitReconnectTimeMilliSeconds)
{
	switch(conf) {
	case(Config::QW):
		return std::make_unique<WriteQImpl>(filename);
	case(Config::NQW):
		return std::make_unique<WriteDirectImpl>(filename, waitConnectTimeMilliSeconds, waitReconnectTimeMilliSeconds);
	default:
		throw std::runtime_error("no Config WriteFactory");
	}
}
} // namespace Ipc