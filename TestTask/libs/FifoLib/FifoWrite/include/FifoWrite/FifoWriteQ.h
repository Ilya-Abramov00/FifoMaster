#ifndef FIFOWRITEQ_H
#define FIFOWRITEQ_H

#include "FifoWrite/IFifoWriter.h"

namespace Ipc {

class QWriteImpl : public IFifoWriter {
public:
	explicit QWriteImpl(const std::string fdFileName);

	void setConnectionHandler(ConnectionHandler handler) override;

	void setDisConnectionHandler(ConnectionHandler handler) override;

	void startWrite() override;

	void stopWrite() override;

	void pushData(const void* data, size_t sizeN) override;

	bool const getWaitDisconnect() const override;

	bool const getWaitConnect() const override;

	long const& getFifoFd() const override;

private:

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
	bool waitOpen{false};

	long fifoFd = -1;
	std::queue<std::vector<uint8_t>> queue;
	std::mutex mtx;
	std::unique_ptr<std::thread> threadWriteFifo;
};

} // namespace Ipc
#endif
