#ifndef FIFOWRITENQ_H
#define FIFOWRITENQ_H

#include "FifoWrite/IFifoWriter.h"

namespace Ipc {

class NQWriteImpl : public IFifoWriter {
public:
	NQWriteImpl(std::string fdFileName,size_t waitMilliSeconds);

	void setConnectionHandler(ConnectionHandler handler) override;

	void setDisConnectionHandler(ConnectionHandler handler) override;

	void startWrite() override;

	void stopWrite() override;

	void pushData(const void* data, size_t sizeN) override;


	bool  getWaitConnect() const override;

	long const& getFifoFd() const override;

private:
	void waitConnectFifo();

	struct Params {
		std::string addrRead;
		ConnectionHandler connectHandler;
		ConnectionHandler disconnectHandler;
	};
	Params params;
	bool waitOpen{false};
	bool waitConnect{false};
	long fifoFd = -1;

	std::unique_ptr<std::thread> threadWaitConnectFifo;
};

} // namespace Ipc
#endif
