#include "Server/Server.h"
#include <iostream>

namespace Ipc {
void Server::getter(size_t id, FifoRead::Data&& data)
{
	readHandler(id, std::move(data));
};

void Server::connectH(ConnectionId id, const Fifo& object)
{
	std::lock_guard<std::mutex> mtx(mtxConnect);
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
	std::lock_guard<std::mutex> mtx(mtxDisconnect);
	if(!object.getWaitConnectWrite() || !object.getWaitConnectRead()) {
		if(stateClient.at(id) == State::connect) {
			stateClient.at(id) = State::disconnect;
			std::cout << "Disconnect " << id << std::endl;
			closeHandler(id);
		}
	}
};

Server::Server(std::list<FifoCfg> const& nameChannelsFifo) : nameChannelsFifo(nameChannelsFifo)
{}
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
	if(!idDistributionHandler) {
		throw std::runtime_error("callback idDistributionHandler not set");
	}
	initialization();

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
void Server::setIdDistributionHandler(Server::IdDistributionHandler h)
{
	idDistributionHandler = std::move(h);
}

void Server::write(size_t id, const void* data, size_t sizeInBytes)
{
	connectionTable.at(id)->write(data, sizeInBytes);
}
Server::~Server()
{
	for(auto& fifoCfg: nameChannelsFifo) {
		unlink(fifoCfg.directFile.c_str());
		unlink(fifoCfg.reverseFile.c_str());
	}
}

void Server::disconnect(size_t id)
{
	connectionTable.at(id)->closeWrite();
	connectionTable.at(id)->closeRead();
}

void Server::initialization()
{
	for(auto name: nameChannelsFifo) {
		size_t id = idDistributionHandler();
		stateClient.insert({id, State::disconnect});

		auto writer = WriterFactory::create(name.reverseFile);

		auto fifo = std::make_unique<Fifo>(std::move(writer), name.directFile);

		fifo->configReconnect();

		fifo->setReadHandler([ id](FifoRead::Data&& data) {
			this->getter(id, std::move(data));
		});

		connectionTable.insert({id, std::move(fifo)});

		connectionTable.at(id)->setConnectionHandlerRead([this, id]() {
			this->connectH(id, *connectionTable.at(id));
		});

		connectionTable.at(id)->setConnectionHandlerWrite([this, id]() {
			this->connectH(id, *connectionTable.at(id));
		});

		connectionTable.at(id)->setDisconnectionHandlerWrite([this, id]() {
			this->disconnectH(id, *connectionTable.at(id));
		});

		connectionTable.at(id)->setDisconnectionHandlerRead([this, id]() {
			connectionTable.at(id)->closeWrite();
			this->disconnectH(id, *connectionTable.at(id));
		});
	}
}

std::unique_ptr<IFifoWriter> Server::WriterFactory::create(std::string filename)

{
	return std::make_unique<WriteQImpl>(filename);
}
} // namespace Ipc