#include "Server/Server.h"
#include <iostream>

namespace Ipc {
    void Server::getter(FifoCfg object, FifoRead::Data &&data) {
        readHandler(object, std::move(data));
    };

    void Server::connect(size_t id, std::shared_ptr<Fifo> object) {
        if (object->getWaitConnectWrite() && object->getWaitConnectRead()) {
            std::cout << "Connect " << id << std::endl;
            connectHandler(id);
        }
    };

    void Server::disconnect(size_t id, std::shared_ptr<Fifo> object) {
        if (object->getWaitDisconnectWrite() || object->getWaitDisconnectRead()) {
            std::cout << "Disconnect " << id << std::endl;
            disconnectHandler(id);
        }
    };

    Server::Server(std::list<FifoCfg> const &nameChannelsfifo) {
        size_t id = 0;
        for (auto const &name: nameChannelsfifo) {

            fifoCfgTable.insert({id, name});

            connectionTable.insert({id, std::make_unique<Fifo>(name.reverseFile, name.directFile)});

            connectionTable[id]->setReadHandler([this, name](FifoRead::Data &&data) {
                this->getter(name, std::move(data));
            });

            connectionTable[id]->setConnectionHandlerRead([this, id]() {
                this->connect(id, connectionTable[id]);
            });

            connectionTable[id]->setDisconnectionHandlerRead([this, id]() {
                connectionTable[id]->closeWrite(); // чтобы не приходилась ждать stop
                this->disconnect(id, connectionTable[id]);
            });

            connectionTable[id]->setConnectionHandlerWrite([this, id]() {
                this->connect(id, connectionTable[id]);
            });

            connectionTable[id]->setDisconnectionHandlerWrite([this, id]() {
                connectionTable[id]->closeRead(); // чтобы не приходилась ждать stop
                this->disconnect(id, connectionTable[id]);
            });
            id++;
        }
    }

    void Server::start() {
        if (!readHandler) {
            throw std::runtime_error("callback readHandler not set");
        }
        if (!connectHandler) {
            throw std::runtime_error("callback connectHandler not set");
        }
        if (!disconnectHandler) {
            throw std::runtime_error("callback disconnectHandler not set");
        }
        for (const auto &Fifo: connectionTable) {
            Fifo.second->start();
        }
    }

    void Server::stop() {
        for (const auto &Fifo: connectionTable) {
            Fifo.second->stop();

        }
    }

    void Server::setReadHandler(ReadHandler h) {
        readHandler = std::move(h);
    }

    void Server::setDisconnectHandler(EventHandler h) {
        disconnectHandler = std::move(h);
    }

    void Server::setConnectHandler(EventHandler h) {
        connectHandler = std::move(h);
    }

    void Server::write(size_t id, const void *data, size_t sizeInBytes) {
        connectionTable[id]->write(data, sizeInBytes);
    }
}