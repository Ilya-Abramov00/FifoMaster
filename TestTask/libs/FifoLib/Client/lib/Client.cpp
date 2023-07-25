#include "Client/Client.h"
#include <iostream>

namespace Ipc {
    Client::Client(FifoCfg name) : client(name.directFile, name.reverseFile) {
        client.setReadHandler([this](FifoRead::Data &&data) {
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

    void Client::start() {
        client.start();
    }

    void Client::write(const void *data, size_t sizeN) {
        client.write(data, sizeN);
    }

    void Client::stop() {
        client.stop();
    }

    void Client::setReadHandler(FifoRead::ReadHandler h) {
        readHandler = std::move(h);
    };

    void Client::getter(FifoRead::Data &&data) {
        readHandler(std::move(data));
    };

    void Client::logicConnect() {
        if (client.getWaitConnectWrite() && client.getWaitConnectRead()) {
            std::cout << "Connect " << std::endl;
        }
    };

    void Client::logicDisConnect() {
        if (client.getWaitDisconnectWrite() || client.getWaitDisconnectRead()) {
            std::cout << "Disconnect " << std::endl;
        }
    }
}