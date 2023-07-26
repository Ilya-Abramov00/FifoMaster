#ifndef FIFOWRITEQ_H
#define FIFOWRITEQ_H

#include "FifoWrite/FifoIWriter.h"

namespace Ipc {

class QWriteImpl : public FifoIWriter, protected FifoBase {
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

} // namespace Ipc
#endif
