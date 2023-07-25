#include "Server/Server.h"
#include <iostream>

void Server::getter(FifoRead::Data&& data)
{
	readHandler(connectionTable, std::move(data));
};

void Server::logicConnect(std::shared_ptr<Fifo> object)
{
	if(object->getWaitConnectWrite() && object->getWaitConnectRead()) {
		std::cout << "Connect " << object->getNameRead()<< std::endl;
	}
};
void Server::logicDisconnect(std::shared_ptr<Fifo> object)
{
	if(object->getWaitDisconnectWrite() || object->getWaitDisconnectRead()) {
		std::cout << "Disconnect " << object->getNameRead()<< std::endl;
	}
};

Server::Server(  ServedFiles nameChannelsfifo)
{
	for(auto  const& name: nameChannelsfifo) {

		connectionTable.insert({name, std::make_unique<Fifo>(name.reverseFile, name.directFile)});

		connectionTable[name]->setReadHandler([this](FifoRead::Data&& data) {
			this->getter(std::move(data));
		});

		connectionTable[name]->setConnectionHandlerRead([this, name]() {
			this->logicConnect(connectionTable[name]);
		});

		connectionTable[name]->setDisConnectionHandlerRead([this, name]() {
			connectionTable[name]->closeWrite();
		this->logicDisconnect(connectionTable[name]);
		});

		connectionTable[name]->setConnectionHandlerWrite([this, name]() {
			this->logicConnect(connectionTable[name]);
		});

		connectionTable[name]->setDisConnectionHandlerWrite([this, name]() {
			connectionTable[name]->closeRead();
			this->logicDisconnect(connectionTable[name]);
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
void Server::setNewConnectionHandler(Server::ConnChangeHandler h)
{
	newHandler = std::move(h);
}

void Server::setCloseConnectionHandler(Server::ConnChangeHandler h)
{
	closeHandler = std::move(h);
}

void Server::write(FifoCfg object, const void* data, size_t sizeInBytes)
{
	connectionTable[object]->write(data, sizeInBytes);
}
