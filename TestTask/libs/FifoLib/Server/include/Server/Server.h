#ifndef SERVER_H
#define SERVER_H

#include "list"
#include "map"
#include "Fifo/Fifo.h"

struct FifoCfg {
	std::string directFile;
	std::string reverseFile;


	bool operator== (const FifoCfg&other) const
	{
		return (this->directFile == other.directFile &&
		        this->reverseFile == other.reverseFile);
	}
	bool operator< (const FifoCfg&other) const
	{
		return (directFile < other.directFile );

	}

};


class Server {
public:
	using ServedFiles = std::list<FifoCfg>;

	using ConnectionsTable      = std::map<FifoCfg, std::shared_ptr<Fifo>>;
	using ConnChangeHandler     = std::function<void(ConnectionsTable)>;
	using ReadHandler           = std::function<void(ConnectionsTable, FifoRead::Data&&)>;
	using IdDistributionHandler = std::function<ConnectionsTable()>;

	Server( ServedFiles  nameChannelsFifo);

	void setReadHandler(ReadHandler h);

	void setNewConnectionHandler(ConnChangeHandler h);
	void setCloseConnectionHandler(ConnChangeHandler h);

	void write(std::shared_ptr<Fifo> object, const void* data, size_t sizeInBytes);

	void start();
	void stop();

	ConnectionsTable connectionTable;
private:

	void getter(FifoRead::Data&& data);
	void logicConnect(std::shared_ptr<Fifo> object);

	void logicDisconnect(std::shared_ptr<Fifo> object);

	ReadHandler readHandler;

	ConnChangeHandler newHandler;
	ConnChangeHandler closeHandler;
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
