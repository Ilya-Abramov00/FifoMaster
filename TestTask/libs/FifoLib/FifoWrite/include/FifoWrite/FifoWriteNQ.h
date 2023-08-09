#ifndef FIFOWRITENQ_H
#define FIFOWRITENQ_H

#include "FifoWrite/IFifoWriter.h"

namespace Ipc {

class NQWriteImpl : public IFifoWriter {
public:
	NQWriteImpl(const std::string fdFileName);

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
	bool runWrite{false};
	enum class State{
		connect,
		disconnect
	};
	State state=State::disconnect;
	long fifoFd = -1;

	std::unique_ptr<std::thread> threadWaitConnectFifo;
};

} // namespace Ipc
#endif
