#ifndef FIFOIWRITE_H
#define FIFOIWRITE_H

#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>
namespace Ipc {

class IWriter {
	virtual void setConnectionHandler(FifoBase::ConnectionHandler handler) = 0;

	virtual void setDisConnectionHandler(FifoBase::ConnectionHandler handler) = 0;

	virtual void startWrite() = 0;

	virtual void stopWrite() = 0;

	virtual void pushData(const void* data, size_t sizeN) = 0;

	virtual bool const getWaitDisconnect() const = 0;

	virtual bool const getWaitConnect() const = 0;

	virtual std::string const getName() const = 0;

	virtual long const& getFifoFd() const = 0;
};
}; // namespace Ipc
#endif
