#ifndef FIFOWRITE_H
#define FIFOWRITE_H

#include "FifoBase/FifoBase.h"

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

class QWriteImpl : public IWriter, protected FifoBase {
public:
	QWriteImpl(const std::string fdFileName);

	void setConnectionHandler(FifoBase::ConnectionHandler handler) override;

	void setDisConnectionHandler(FifoBase::ConnectionHandler handler) override;

	void startWrite() override;

	void stopWrite() override;

	void pushData(const void* data, size_t sizeN) override;

	bool const getWaitDisconnect() const override;

	bool const getWaitConnect() const override;

	std::string const getName() const override;

	long const& getFifoFd() const override;

private:
	void waitConnectFifo();

	void writeFifo();

	struct Params {
		std::string addrRead;
		ConnectionHandler connectHandler;
		ConnectionHandler disconnectHandler;
	};
	Params params;
	bool runWrite{false};
	bool waitConnect{false};
	bool waitDisConnect{false};
	long fifoFd = -1;
	std::queue<std::vector<uint8_t>> queue;
	std::mutex mtx;
	std::unique_ptr<std::thread> threadWriteFifo;
	std::unique_ptr<std::thread> threadWaitConnectFifo;
};

class NQWriteImpl : public IWriter, protected FifoBase {
public:
	NQWriteImpl(const std::string fdFileName);

	void setConnectionHandler(FifoBase::ConnectionHandler handler) override;

	void setDisConnectionHandler(FifoBase::ConnectionHandler handler) override;

	void startWrite() override;

	void stopWrite() override;

	void pushData(const void* data, size_t sizeN) override;

	bool const getWaitDisconnect() const override;

	bool const getWaitConnect() const override;

	std::string const getName() const override;

	long const& getFifoFd() const override;

private:
	void waitConnectFifo();

	struct Params {
		std::string addrRead;
		ConnectionHandler connectHandler;
		ConnectionHandler disconnectHandler;
	};
	Params params;
	bool runWrite{false};
	bool waitConnect{false};
	bool waitDisConnect{false};
	long fifoFd = -1;

	std::unique_ptr<std::thread> threadWaitConnectFifo;
};

} // namespace Ipc
#endif
