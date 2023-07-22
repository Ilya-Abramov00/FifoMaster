#include "Server/Server.h"

void Server::getter(FifoRead::Data&& data)
{
	readHandler(connectionId, std::move(data));
};

void Server::logicConnect(std::shared_ptr<Fifo> object)
{
	if(object->getWaitConnectWrite() || object->getWaitConnectRead()) {
		std::cout << "произошел Connect" << std::endl;
	}
};
void Server::logicDisConnect(std::shared_ptr<Fifo> object)
{
	if(object->getWaitDisConnectRead() || object->getWaitDisConnectWrite()) {
		std::cout << "произошел Disconnect" << std::endl;
	}
};

Server::Server(const std::vector<std::string>& nameChannelsfifo) : nameChannelsFifo(nameChannelsfifo)
{
	for(auto const& name: nameChannelsfifo) {
		connectionId[name] = std::make_unique<Fifo>(name, name + "_reverse");

		connectionId[name]->setReadHandler([this](FifoRead::Data&& data) {
			this->getter(std::move(data));
		});

		connectionId[name]->setConnectionHandlerRead([this, name]() {
			this->logicConnect(connectionId[name]);
		});
		connectionId[name]->setDisConnectionHandlerRead([this, name]() {
			this->logicDisConnect(connectionId[name]);
		});
		connectionId[name]->setConnectionHandlerWrite([this, name]() {
			this->logicConnect(connectionId[name]);
		});
		connectionId[name]->setDisConnectionHandlerWrite([this, name]() {
			this->logicDisConnect(connectionId[name]);
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

void Server::write(std::shared_ptr<Fifo> object, const void* data, size_t sizeInBytes)
{
	object->write(data, sizeInBytes);
}
