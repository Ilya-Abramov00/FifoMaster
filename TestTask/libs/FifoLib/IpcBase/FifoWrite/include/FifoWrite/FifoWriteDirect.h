#ifndef FIFOWRITENQ_H
#define FIFOWRITENQ_H

#include "FifoWrite/IFifoWriter.h"

namespace Ipc {

class WriteDirectImpl : public IFifoWriter {
public:
	WriteDirectImpl(std::string fdFileName, size_t waitTimeConnectMilliSeconds);

	void setConnectionHandler(ConnectionHandler handler) override;

	void setDisconnectionHandler(ConnectionHandler handler) override;

	void startWrite() override;

	void stopWrite() override;

	void pushData(const void* data, size_t sizeN) override;

	bool getWaitConnect() const override;

	long const& getFifoFd() const override;

	void reconnectTrue() override;;

private:
	void connect() ;

	struct Params {
		std::string addrRead;
		ConnectionHandler connectHandler;
		ConnectionHandler disconnectHandler;
		size_t waitConnnectTimeMilliSeconds;
	};
	Params params;
	bool waitOpen{false};
	bool waitConnect{false};
	long fifoFd = -1;

	std::unique_ptr<std::thread> threadWaitConnectFifo;
};

} // namespace Ipc
#endif
