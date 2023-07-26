#ifndef FIFOREAD_H
#define FIFOREAD_H

#include "FifoBase/FifoBase.h"

#include <string>
#include <functional>
#include <thread>

namespace Ipc {
    class FifoRead : protected FifoBase {
    public:
        using Data = std::vector<uint8_t>;
        using ReadHandler = std::function<void(Data &&)>;

        FifoRead(const std::string fdFileName);

        void startRead();

        void stopRead();

        void setConnectionHandler(FifoBase::ConnectionHandler handler);

        void setDisConnectionHandler(FifoBase::ConnectionHandler handler);

        void setReadHandler(ReadHandler handler);

        bool const getWaitDisconnect() const;

        bool const getWaitConnect() const;

        std::string const getName() const;

        long const &getFifoFd() const;

    private:
        void waitConnectFifo();

        void readFifo();

        struct Params {
            std::string addrRead;
            ReadHandler msgHandler;
            ConnectionHandler connectHandler;
            ConnectionHandler disconnectHandler;
        };
        Params params;
        bool runRead{false};
        bool waitConnect{false};
        bool waitDisConnect{false};
        long fifoFd = -1;

        std::unique_ptr<std::thread> threadReadFifo;
    };
}
#endif
