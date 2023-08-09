#include "Client/Client.h"
#include <iostream>

namespace Ipc {
Client::Client(FifoCfg name, Config config) : client(WriterFactory::create(name.directFile, config), name.reverseFile)
{
	client.setReadHandler([this](FifoRead::Data&& data) {
		this->getter(std::move(data));
	});

	client.setConnectionHandlerRead([this]() {
		this->logicConnect();
	});
	client.setDisconnectionHandlerRead([this]() {
		client.closeWrite();
		this->logicDisConnect();
	});
	client.setConnectionHandlerWrite([this]() {
		this->logicConnect();
	});
	client.setDisconnectionHandlerWrite([this]() {
		client.closeRead();
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
	logicDisConnect();
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
	if((client.getWaitConnectWrite()) || client.getWaitConnectRead()) {
		if(state == State::disconnect) {
			state = State::connect;
			std::cout << "Connect " << std::endl;
			connectionHandler();
		}
	}
};

void Client::logicDisConnect()
{
	if((client.getWaitDisconnectRead()) || client.getWaitDisconnectWrite()) {
		if(state == State::connect) {
			state = State::disconnect;
			std::cout << "Disconnect " << std::endl;
			disconnectionHandler();
		}
	}
}
std::unique_ptr<IFifoWriter> Client::WriterFactory::create(const std::string& filename, Config conf)
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
