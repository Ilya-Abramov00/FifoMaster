#ifndef SERVER_H
#define SERVER_H

#include "list"
#include "map"
#include "Fifo/Fifo.h"



class Server {
public:
	using ServerFiles           = std::list<FifoCfg>;
	using FifoCfgTable      = std::map<size_t ,FifoCfg>;

	using ConnectionsTable      = std::map<size_t, std::shared_ptr<Fifo>>;

	using ReadHandler           = std::function<void(FifoCfg, FifoRead::Data&&)>;

	using EventHandler = std::function<void(size_t)>;

	Server(ServerFiles const & nameChannelsFifo);

	void setReadHandler(ReadHandler h);

	void setConnectHandler(EventHandler h);
	void setDisconnectHandler(EventHandler h);

	void write(size_t id, const void* data, size_t sizeInBytes);

	void start();
	void stop();

private:
	FifoCfgTable fifoCfgTable;
	ConnectionsTable connectionTable;

	EventHandler connectHandler;
	EventHandler disconnectHandler;

	void getter(FifoCfg object,FifoRead::Data&& data);

	void connect(size_t id,std::shared_ptr<Fifo> object);
	void disconnect(size_t id,std::shared_ptr<Fifo> object);

	ReadHandler readHandler;


};

// virtual ~Server() = default;
// virtual void start()                                             = 0;
// virtual void stop()                                              = 0;
// virtual void write(ConnectionsTable id, const void* data, size_t sz) = 0;
// virtual void disconnect(ConnectionsTable id)                         = 0;
// void setNewConnectionHandler(ConnChangeHandler h)
//{
//	newHandler = h;
// }
// void setCloseConnectionHandler(ConnChangeHandler h)
//{
//	closeHandler = h;
// }

// virtual void setIdDistributionHandler(IdDistributionHandler h) = 0;
#endif
