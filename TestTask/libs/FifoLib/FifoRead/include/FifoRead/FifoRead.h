#ifndef FIFOREAD_H
#define FIFOREAD_H

#include "FifoBase/FifoBase.h"


class FifoRead : protected FifoBase {
public:
	using Data        = std::vector<uint8_t>;
	using ReadHandler = std::function<void(Data&&)>;

	FifoRead(const std::string fdFileName);
	void startRead();
	void stopRead();

	void setConnectionHandler(FifoBase::ConnectionHandler handler);
	void setDisConnectionHandler(FifoBase::ConnectionHandler handler);

	void setReadHandler(ReadHandler handler);
	~FifoRead();
	bool const getWaitDisConnect() const
	{
		return waitDisConnect;
	}
	bool const getWaitConnect() const
	{
		return waitConnect;
	}

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
	long fifoReadFd = -1;
	std::unique_ptr<std::thread> threadWaitConnectFifo;
	std::unique_ptr<std::thread> threadReadFifo;
};

#endif
