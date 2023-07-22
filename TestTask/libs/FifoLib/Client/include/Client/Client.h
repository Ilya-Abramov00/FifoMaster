#ifndef Client_H
#define Client_H


#include "Fifo/Fifo.h"

class Client :public Fifo{
public:
	using ConnectionId          = std::unordered_map<std::string, std::shared_ptr<Fifo>>;
	using ConnChangeHandler     = std::function<void(ConnectionId)>;
	using ReadHandler           = std::function<void(ConnectionId, FifoRead::Data&&)>;
	using IdDistributionHandler = std::function<ConnectionId()>;

	Client(std::string nameChannel):Fifo(nameChannel,nameChannel+"_reverse");

private:

};


#endif
