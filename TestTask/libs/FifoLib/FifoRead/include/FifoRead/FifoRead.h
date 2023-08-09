#ifndef FIFOREAD_H
#define FIFOREAD_H

#include "FifoBase/FifoBase.h"
#include <vector>
#include <string>
#include <functional>
#include <thread>

namespace Ipc {
class FifoRead {
public:
	using Data        = std::vector<uint8_t>;
	using ReadHandler = std::function<void(Data&&)>;

	FifoRead(const std::string fdFileName);

	void startRead();

	void stopRead();

	void setConnectionHandler(ConnectionHandler handler);

	void setDisConnectionHandler(ConnectionHandler handler);

	void setReadHandler(ReadHandler handler);

	bool const getWaitConnect() const;

	long const& getFifoFd() const;

private:


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

	bool waitOpen{false};

	long fifoFd = -1;

	std::unique_ptr<std::thread> threadReadFifo;
};
} // namespace Ipc
#endif
