#ifndef FIFOWRITE_H
#define FIFOWRITE_H

#include "FifoBase/FifoBase.h"

class FifoWrite : protected FifoBase {
public:
	void setConnectionHandler(FifoBase::ConnectionHandler handler);
	void setDisConnectionHandler(FifoBase::ConnectionHandler handler);

	void startWrite();
	void stopWrite();

	FifoWrite(const std::string fdFileName);

	void pushData(const void* data, size_t sizeN);

	bool const getWaitDisConnect() const
	{
		return waitDisConnect;
	}
	bool const getWaitConnect() const
	{
		return waitConnect;
	}
	std::string const getName()const{
		return params.addrRead;
	}

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
	std::queue<std::vector<uint8_t>> queue;
	long fifoFd = -1;
	std::mutex mtx;
	std::unique_ptr<std::thread> threadWriteFifo;
	std::unique_ptr<std::thread> threadWaitConnectFifo;
};

#endif
