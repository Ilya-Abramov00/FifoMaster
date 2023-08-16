#ifndef Client_H
#define Client_H

#include "Fifo/Fifo.h"
#include "optional"
namespace Ipc {
class Client {
	using ConnChangeHandler = std::function<void()>;
	using ReadHandler       = std::function<void(FifoRead::Data&&)>;

public:
	Client(FifoCfg name, Config config, std::optional<size_t> waitConnectTimeMilliSeconds);

	void start();

	void stop();

	void write(const void* data, size_t sizeN);

	void setReadHandler(ReadHandler h);

	void setNewHandler(ConnChangeHandler h);

	void setCloseHandler(ConnChangeHandler h);

private:
	void logicConnect();

	void logicDisConnect();

	void getter(FifoRead::Data&& data);

	ConnChangeHandler newHandler;
	ConnChangeHandler closeHandler;
	ReadHandler readHandler;

	enum class State { disconnect, connect };
	State state{State::disconnect};
	Fifo client;

	class WriterFactory {
	public:
		static std::unique_ptr<IFifoWriter> create(const std::string& filename, Config conf,
		                                           size_t waitConnectTimeMilliSeconds);
	};
};
} // namespace Ipc

#endif
