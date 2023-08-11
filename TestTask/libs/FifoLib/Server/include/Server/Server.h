#ifndef SERVER_H
#define SERVER_H

#include "Fifo/Fifo.h"
#include "FifoWrite/FifoWriteQ.h"
#include "FifoWrite/FifoWriteDirect.h"
#include "FifoWrite/IFifoWriter.h"

#include <list>
#include <map>
#include <optional>

namespace Ipc {

class Server {
public:
	using ConnectionId      = size_t;
	using ReadHandler       = std::function<void(ConnectionId, FifoRead::Data&&)>;
	using ConnChangeHandler = std::function<void(ConnectionId)>;

	using FifoCfgTable     = std::map<size_t, FifoCfg>;
	using ConnectionsTable = std::map<size_t, std::unique_ptr<Fifo>>;

	Server(std::list<FifoCfg> const& nameChannelsFifo, Config config, std::optional<size_t> waitConnectTimeMilliSeconds,
	       std::optional<size_t> waitReconnectTimeMilliSeconds);

	void setReadHandler(ReadHandler h);

	void setConnectHandler(ConnChangeHandler h);

	void setDisconnectHandler(ConnChangeHandler h);

	void write(size_t id, const void* data, size_t sizeInBytes);

	void start();

	void stop();

	void disconnect(ConnectionId id);

	~Server();

private:
	FifoCfgTable fifoCfgTable;
	ConnectionsTable connectionTable;

	enum class State { disconnect, connect };
	std::vector<State> stateClient;

	std::mutex mtxConnect;
	std::mutex mtxDisconnect;

	ConnChangeHandler newHandler;
	ConnChangeHandler closeHandler;

	ReadHandler readHandler;
	ConnectionId idCount = 0;

	void getter(ConnectionId id, FifoRead::Data&& data);

	void connectH(ConnectionId id, const Fifo& object);

	void disconnectH(ConnectionId id, Fifo& object);

	class WriterFactory {
	public:
		static std::unique_ptr<IFifoWriter> create(std::string filename, Config conf,
		                                           size_t waitConnectTimeMilliSeconds,
		                                           size_t waitReconnectTimeMilliSeconds);
	};
};
} // namespace Ipc

#endif
