#ifndef Client_H
#define Client_H

#include "Fifo/Fifo.h"

namespace Ipc {
class Client {
	using EventHandler = std::function<void()>;

public:
	Client(FifoCfg name, Config config);

	void start();

	void stop();

	void write(const void* data, size_t sizeN);

	void setReadHandler(FifoRead::ReadHandler h);

	void setConnectHandler(EventHandler h);

	void setDisconnectHandler(EventHandler h);

private:
	void logicConnect();

	void logicDisConnect();

	void getter(FifoRead::Data&& data);

	EventHandler connectionHandler;
	EventHandler disconnectionHandler;

	Fifo client;
	FifoRead::ReadHandler readHandler;

	class WriterFactory {
	public:
		static std::unique_ptr<FifoIWriter> create(const std::string& filename, Config conf);
	};
};
} // namespace Ipc

#endif
