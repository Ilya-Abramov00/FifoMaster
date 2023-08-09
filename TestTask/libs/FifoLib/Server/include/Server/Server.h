#ifndef SERVER_H
#define SERVER_H

#include "list"
#include "map"
#include "Fifo/Fifo.h"
#include "FifoWrite/FifoWriteQ.h"
#include "FifoWrite/FifoWriteNQ.h"
#include "FifoWrite/IFifoWriter.h"
namespace Ipc {

class Server {
public:
	using FifoCfgTable     = std::map<size_t, FifoCfg>;
	using ConnectionsTable = std::map<size_t, std::unique_ptr<Fifo>>;

	using ReadHandler  = std::function<void(size_t, FifoRead::Data&&)>;
	using EventHandler = std::function<void(size_t)>;

	Server(std::list<FifoCfg> const& nameChannelsFifo, Config config);

	void setReadHandler(ReadHandler h);

	void setConnectHandler(EventHandler h);

	void setDisconnectHandler(EventHandler h);

	void writeId(size_t id, const void* data, size_t sizeInBytes);

	void start();

	void stop();

    void startId(size_t id);
    void stopId(size_t id);


	~Server();

private:
	FifoCfgTable fifoCfgTable;
	ConnectionsTable connectionTable;

	enum class State { disconnect, connect };
	std::vector<State> stateClient;

	std::mutex mtxConnect;
	std::mutex mtxDisconnect;
	EventHandler connectHandler;
	EventHandler disconnectHandler;

	ReadHandler readHandler;
	size_t idCount = 0;

	void getter(size_t id, FifoRead::Data&& data);

	void connect(size_t id, const Fifo& object);

	void disconnect(size_t id, Fifo& object);

	class WriterFactory {
	public:
		static std::unique_ptr<IFifoWriter> create(std::string filename, Config conf);
	};
};
} // namespace Ipc

#endif
