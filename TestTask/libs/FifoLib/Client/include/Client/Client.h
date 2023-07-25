#ifndef Client_H
#define Client_H

#include "Fifo/Fifo.h"
namespace Ipc {
    class Client {
        using EventHandler = std::function<void()>;
    public:
        Client(FifoCfg name);

        void start();

        void stop();

        void write(const void *data, size_t sizeN);

        void setReadHandler(FifoRead::ReadHandler h);

        void setConnectHandler(EventHandler h);

        void setDisconnectHandler(EventHandler h);

    private:
        void logicConnect();

        void logicDisConnect();

        void getter(FifoRead::Data &&data);

        EventHandler connectionHandler;
        EventHandler disconnectionHandler;

        Fifo client;
        FifoRead::ReadHandler readHandler;
    };
}

#endif
