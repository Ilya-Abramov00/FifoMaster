#ifndef Client_H
#define Client_H

#include "Fifo/Fifo.h"
#include "optional"
namespace Ipc {
class Client {
	using EventHandler = std::function<void()>;

public:
	Client(FifoCfg name, Config config, std::optional<size_t> waitConnectTimeMilliSeconds,
	       std::optional<size_t> waitReconnectTimeMilliSeconds);

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

	enum class State { disconnect, connect };
	State state{State::disconnect};
	Fifo client;
	FifoRead::ReadHandler readHandler;

	class WriterFactory {
	public:
		static std::unique_ptr<IFifoWriter> create(const std::string& filename, Config conf,
		                                           size_t waitConnectTimeMilliSeconds,
		                                           size_t waitReconnectTimeMilliSeconds);
	};
};
} // namespace Ipc

#endif
