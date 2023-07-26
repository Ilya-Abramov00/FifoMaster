#include "FifoWrite/FifoWrite.h"



#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <mutex>
#include <queue>
#include <sys/signal.h>

namespace Ipc {
    QWriteImpl::QWriteImpl(std::string fdFileName) {
        params.addrRead = fdFileName;
        createFifo(params.addrRead);
    }

    void QWriteImpl::setConnectionHandler(FifoBase::ConnectionHandler handler) {
        params.connectHandler = std::move(handler);
    }

    void QWriteImpl::setDisConnectionHandler(FifoBase::ConnectionHandler handler) {
        params.disconnectHandler = std::move(handler);
    }

    void QWriteImpl::startWrite() {
        if (!params.connectHandler) {
            throw std::runtime_error("callback Write connectHandler not set");
        }
        if (!params.disconnectHandler) {
            throw std::runtime_error("callback Write disconnectHandler not set");
        }
        runWrite = true;

        threadWaitConnectFifo = std::make_unique<std::thread>(std::thread([this]() {
            waitConnectFifo();
        }));
    }

    void QWriteImpl::waitConnectFifo() {
        fifoFd = openFifo(params.addrRead, 'W');
        if (runWrite) {
            waitConnect = true;
            params.connectHandler();
            threadWriteFifo = std::make_unique<std::thread>(std::thread([this]() {
                writeFifo();
            }));
        }
    }

    void QWriteImpl::stopWrite() {
        runWrite = false;
        if (!waitConnect) {
            auto fd = openFifo(params.addrRead.c_str(), 'R');
            close(fd);
        }

        threadWaitConnectFifo->join();
        close(fifoFd);

        if (waitConnect) {
            threadWriteFifo->join();
        }
        if (queue.size()) {
            std::cerr << getName() << " канал не смог отправить данные в полном размере";
        }
    }

    void QWriteImpl::writeFifo() {
        if (fifoFd == -1) {
            throw std::runtime_error(" fail openFifo ");
        }
        while (runWrite) {
            {
                std::lock_guard<std::mutex> mtx_0(mtx);
                if (!queue.empty()) {
                    signal(SIGPIPE, SIG_IGN); // отлавливает сигнал в случае закрытия канала на чтение
                    auto flag = write(fifoFd, queue.front().data(), queue.front().size());
                    if (flag == -1) {
                        waitDisConnect = true;
                        params.disconnectHandler();
                        return;
                    }
                    queue.pop();
                }
            }
        }
    }

    void QWriteImpl::pushData(const void *data, size_t sizeN) {
        if (!data) {
            std::cerr << "\n null ptr is pushData \n";
            return;
        }
        auto ptr = reinterpret_cast<const uint8_t *>(data);
        std::vector<uint8_t> buffer(ptr, ptr + sizeN);
        {
            std::lock_guard<std::mutex> mtx_0(mtx);
            queue.push(std::move(buffer));
        }
    }

    bool const QWriteImpl::getWaitDisconnect() const {
        return waitDisConnect;
    }

    std::string const QWriteImpl::getName() const {
        return params.addrRead;
    }

    long const &QWriteImpl::getFifoFd() const {
        return fifoFd;
    }

    bool const QWriteImpl::getWaitConnect() const {
        return waitConnect;
    }


    NQWriteImpl:: NQWriteImpl(std::string fdFileName) {
        params.addrRead = fdFileName;
        createFifo(params.addrRead);
    }

    void  NQWriteImpl::setConnectionHandler(FifoBase::ConnectionHandler handler) {
        params.connectHandler = std::move(handler);
    }

    void  NQWriteImpl::setDisConnectionHandler(FifoBase::ConnectionHandler handler) {
        params.disconnectHandler = std::move(handler);
    }

    void  NQWriteImpl::startWrite() {
        if (!params.connectHandler) {
            throw std::runtime_error("callback Write connectHandler not set");
        }
        if (!params.disconnectHandler) {
            throw std::runtime_error("callback Write disconnectHandler not set");
        }
        runWrite = true;

        threadWaitConnectFifo = std::make_unique<std::thread>(std::thread([this]() {
            waitConnectFifo();
        }));
    }

    void  NQWriteImpl::waitConnectFifo() {
        fifoFd = openFifo(params.addrRead, 'W');
        if (runWrite) {
            waitConnect = true;
            params.connectHandler();
        }
    }

    void  NQWriteImpl::stopWrite() {
        runWrite = false;
        if (!waitConnect) {
            auto fd = openFifo(params.addrRead.c_str(), 'R');
            close(fd);
        }
        threadWaitConnectFifo->join();
        close(fifoFd);
    }


    void  NQWriteImpl::pushData(const void *data, size_t sizeN) {
        if (!data) {
            std::cerr << "\n null ptr is pushData \n";
            return;
        }
        if (waitConnect && runWrite) {
            signal(SIGPIPE, SIG_IGN); // отлавливает сигнал в случае закрытия канала на чтение
            auto flag = write(fifoFd, data, sizeN);
            if (flag == -1) {
                waitDisConnect = true;
                params.disconnectHandler();
                return;
            }
        }
    }

    bool const  NQWriteImpl::getWaitDisconnect() const {
        return waitDisConnect;
    }

    std::string const  NQWriteImpl::getName() const {
        return params.addrRead;
    }

    long const &NQWriteImpl::getFifoFd() const {
        return fifoFd;
    }

    bool const  NQWriteImpl::getWaitConnect() const {
        return waitConnect;
    }
}