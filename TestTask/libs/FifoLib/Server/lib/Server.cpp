#include "Server/Server.h"
#include <iostream>

namespace Ipc {
void Server::getter(size_t id, FifoRead::Data&& data)
{
	readHandler(id, std::move(data));
};

void Server::connect(size_t id, const Fifo& object)
{
	if(object.getWaitConnectWrite() && object.getWaitConnectRead()) {
		std::cout << "Connect " << id << std::endl;
		connectHandler(id);
	}
};

void Server::disconnect(size_t id, const Fifo& object)
{
	if(object.getWaitDisconnectWrite() || object.getWaitDisconnectRead()) {
		std::cout << "Disconnect " << id << std::endl;
		disconnectHandler(id);
	}
};

Server::Server(std::list<FifoCfg> const& nameChannelsfifo, Config config)
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
			// чтобы не приходилась ждать stop
			connectionTable[id]->closeWrite();
				this->disconnect(id, *connectionTable[id]);
		});

		connectionTable[id]->setConnectionHandlerWrite([this, id]() {
			this->connect(id, *connectionTable[id]);
		});

		connectionTable[id]->setDisconnectionHandlerWrite([this, id]() {
			connectionTable[id]->closeRead(); // чтобы не приходилась ждать stop
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
std::unique_ptr<IFifoWriter> Server::WriterFactory::create(std::string filename, Config conf)
{
	switch(conf) {
	case(Config::QW):
		return std::make_unique<QWriteImpl>((filename));
	case(Config::NQW):
		return std::make_unique<NQWriteImpl>((filename));
	default:
		throw std::runtime_error("no Config WriteFactory");
	}

}
} // namespace Ipc