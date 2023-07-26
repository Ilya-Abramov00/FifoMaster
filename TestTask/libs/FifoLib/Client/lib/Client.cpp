#include "Client/Client.h"
#include <iostream>

namespace Ipc {
Client::Client(FifoCfg name) : client(name.directFile, name.reverseFile)
{
	client.setReadHandler([this](FifoRead::Data&& data) {
		this->getter(std::move(data));
	});

	client.setConnectionHandlerRead([this]() {
		this->logicConnect();
	});
	client.setDisconnectionHandlerRead([this]() {
		this->logicDisConnect();
	});
	client.setConnectionHandlerWrite([this]() {
		this->logicConnect();
	});
	client.setDisconnectionHandlerWrite([this]() {
		this->logicDisConnect();
	});
}

void Client::start()
{
	if(!readHandler) {
		throw std::runtime_error("callback readHandler not set");
	}
	if(!connectionHandler) {
		throw std::runtime_error("callback connectHandler not set");
	}
	if(!disconnectionHandler) {
		throw std::runtime_error("callback disconnectHandler not set");
	}
	client.start();
}

void Client::write(const void* data, size_t sizeN)
{
	client.write(data, sizeN);
}

void Client::stop()
{
	client.stop();
}

void Client::setReadHandler(FifoRead::ReadHandler h)
{
	readHandler = std::move(h);
};

void Client::setConnectHandler(Ipc::Client::EventHandler h)
{
	connectionHandler = std::move(h);
};

void Client::setDisconnectHandler(Ipc::Client::EventHandler h)
{
	disconnectionHandler = std::move(h);
};

void Client::getter(FifoRead::Data&& data)
{
	readHandler(std::move(data));
};

void Client::logicConnect()
{
	if(client.getWaitConnectWrite() && client.getWaitConnectRead()) {
		std::cout << "Connect " << std::endl;
		connectionHandler();
	}
};

void Client::logicDisConnect()
{
	if(client.getWaitDisconnectWrite() || client.getWaitDisconnectRead()) {
		std::cout << "Disconnect " << std::endl;
		disconnectionHandler();
	}
}
} // namespace Ipc
