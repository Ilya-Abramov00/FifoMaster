#include "Client/Client.h"
#include <iostream>
#include <future>
#include <cassert>

namespace Ipc {

Client::Client(Ipc::FifoCfg name, Ipc::Config config, std::optional<size_t> waitConnectTimeMilliSeconds) :
    client(WriterFactory::create(name.directFile, config, waitConnectTimeMilliSeconds.value()), name.reverseFile)
{
	client.setReadHandler([this](FifoRead::Data&& data) {
		this->getter(std::move(data));
	});

	client.setConnectionHandlerRead([this]() {
		this->logicConnect();
	});

	client.setConnectionHandlerWrite([this]() {
		this->logicConnect();
	});

	client.setDisconnectionHandlerRead([this]() {
		client.closeWrite();
		this->logicDisConnect();
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
	if(!newHandler) {
		throw std::runtime_error("callback newHandler not set");
	}
	if(!closeHandler) {
		throw std::runtime_error("callback closeHandler not set");
	}
	client.start();
}

void Client::write(const void* data, size_t sizeN)
{
	client.write(data, sizeN);
}

void Client::stop()
{
	bool flag{false};
	std::future t = std::async([this, &flag]() {
		client.stop();
		flag = true;
	});

	t.wait_for(std::chrono::seconds(3)); // обработка кейса, когда сервер удалил каналы(вызвал деструтор)(клиент в таком
	                                     // случае не мог бы отключиться)

	if(!flag) {
		throw std::runtime_error("the server crashed");
	}
}

void Client::setReadHandler(ReadHandler h)
{
	readHandler = std::move(h);
};

void Client::setConnectHandler(Ipc::Client::ConnChangeHandler h)
{
	newHandler = std::move(h);
};

void Client::setDisconnectHandler(Ipc::Client::ConnChangeHandler h)
{
	closeHandler = std::move(h);
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
			newHandler();
		}
	}
};

void Client::logicDisConnect()
{
	if((!client.getWaitConnectRead()) || !client.getWaitConnectWrite()) {
		if(state == State::connect) {
			state = State::disconnect;
			std::cout << "Disconnect " << std::endl;
			closeHandler();
		}
	}
}
std::unique_ptr<IFifoWriter> Client::WriterFactory::create(const std::string& filename, Config conf,
                                                           size_t waitConnectTimeMilliSeconds)
{
	switch(conf) {
	case(Config::QW):
		return std::make_unique<WriteQImpl>((filename));
	case(Config::NQW):
		return std::make_unique<WriteDirectImpl>(filename, waitConnectTimeMilliSeconds);
	default:
		throw std::runtime_error("no Config WriteFactory");
	}
}
} // namespace Ipc
