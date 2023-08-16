#ifndef FIFOIWRITE_H
#define FIFOIWRITE_H

#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include "FifoBase/FifoBase.h"

namespace Ipc {

class IFifoWriter {
public:
	virtual void setConnectionHandler(ConnectionHandler handler) = 0;

	virtual void setDisconnectionHandler(ConnectionHandler handler) = 0;

	virtual void startWrite() = 0;

	virtual void stopWrite() = 0;

	virtual void pushData(const void* data, size_t sizeN) = 0;

	virtual bool getWaitConnect() const = 0;

	virtual long const& getFifoFd() const = 0;
	virtual void reconnectTrue()          = 0;

	virtual ~IFifoWriter() = default;
};
}; // namespace Ipc
#endif
