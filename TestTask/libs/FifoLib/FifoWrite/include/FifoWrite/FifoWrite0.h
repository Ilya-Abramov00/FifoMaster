#ifndef FIFOWRITE_H
#define FIFOWRITE_H

#include "FifoBase/FifoBase.h"

#include <string>
#include <functional>
#include <thread>

class FifoWrite : protected FifoBase {
public:
	void setConnectionHandler(FifoBase::ConnectionHandler handler);
	void setDisConnectionHandler(FifoBase::ConnectionHandler handler);

	void startWrite();
	void stopWrite();

	FifoWrite(const std::string fdFileName);

	void pushData(const void* data, size_t sizeN);

	bool const getWaitDisconnect() const;
	bool const getWaitConnect() const;

	std::string const getName() const;

	long const& getFifoFd() const;

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

#endif
