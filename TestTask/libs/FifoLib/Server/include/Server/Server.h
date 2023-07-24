#ifndef SERVER_H
#define SERVER_H


#include "Fifo/Fifo.h"

class Server {
public:
	using ConnectionId          = std::unordered_map<std::string, std::shared_ptr<Fifo>>;
	using ConnChangeHandler     = std::function<void(ConnectionId)>;
	using ReadHandler           = std::function<void(ConnectionId, FifoRead::Data&&)>;
	using IdDistributionHandler = std::function<ConnectionId()>;

	Server(const std::vector<std::string>& nameChannelsFifo);

	void setReadHandler(ReadHandler h);

	void setNewConnectionHandler(ConnChangeHandler h);
	void setCloseConnectionHandler(ConnChangeHandler h);

	void write(std::shared_ptr<Fifo> object, const void* data, size_t sizeInBytes);

	void start();
	void stop();
	ConnectionId connectionId;

private:
	void getter(FifoRead::Data&& data);

	void logicConnect(std::shared_ptr<Fifo> object);
	void logicDisconnect(std::shared_ptr<Fifo> object);

	const std::vector<std::string>& nameChannelsFifo;

	ReadHandler readHandler;

	ConnChangeHandler newHandler;
	ConnChangeHandler closeHandler;
};

// virtual ~Server() = default;
// virtual void start()                                             = 0;
// virtual void stop()                                              = 0;
// virtual void write(ConnectionId id, const void* data, size_t sz) = 0;
// virtual void disconnect(ConnectionId id)                         = 0;
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
