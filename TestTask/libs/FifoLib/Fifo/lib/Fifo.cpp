#include "Fifo/Fifo.h"

namespace Ipc {
    Fifo::Fifo(std::unique_ptr<FifoIWriter> fifoWrite, const std::string fdFileNameRead) :
            fifoRead(fdFileNameRead), fifoWrite(std::move(fifoWrite)) {}

    void Fifo::setReadHandler(FifoRead::ReadHandler handler) {
        fifoRead.setReadHandler(std::move(handler));
    }

    void Fifo::setConnectionHandlerRead(FifoBase::ConnectionHandler handler) {
        fifoRead.setConnectionHandler(std::move(handler));
    }

    void Fifo::setDisconnectionHandlerRead(FifoBase::ConnectionHandler handler) {
        fifoRead.setDisConnectionHandler(std::move(handler));
    }

    void Fifo::setConnectionHandlerWrite(FifoBase::ConnectionHandler handler) {
        fifoWrite->setConnectionHandler(std::move(handler));
    }

    void Fifo::setDisconnectionHandlerWrite(FifoBase::ConnectionHandler handler) {
        fifoWrite->setDisConnectionHandler(std::move(handler));
    }

    void Fifo::write(const void *data, size_t sizeInBytes) {
        fifoWrite->pushData(std::move(data), sizeInBytes);
    }

    void Fifo::stop() {
        fifoWrite->stopWrite();
        fifoRead.stopRead();
    }

    void Fifo::start() {
        fifoRead.startRead();
        fifoWrite->startWrite();
    }

    bool const Fifo::getWaitDisconnectRead() const {
        return fifoRead.getWaitDisconnect();
    }

    bool const Fifo::getWaitConnectRead() const {
        return fifoRead.getWaitConnect();
    }

    bool const Fifo::getWaitDisconnectWrite() const {
        return fifoWrite->getWaitDisconnect();
    }

    bool const Fifo::getWaitConnectWrite() const {
        return fifoWrite->getWaitConnect();
    }

    std::string const Fifo::getNameRead() const {
        return fifoRead.getName();
    }

    std::string const Fifo::getNameWrite() const {
        return fifoWrite->getName();
    }

    void Fifo::closeRead() {
        close(fifoRead.getFifoFd());
    }

    void Fifo::closeWrite() {
        close(fifoWrite->getFifoFd());
    }
}