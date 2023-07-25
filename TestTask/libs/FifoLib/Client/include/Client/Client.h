#ifndef Client_H
#define Client_H

#include "Fifo/Fifo.h"

class Client {
public:
	Client(FifoCfg name);
	void start();
	void stop();
	void write(const void* data, size_t sizeN);
	void setReadHandler(FifoRead::ReadHandler h);
	//добавить callback на коннект.дис
private:
	void logicConnect();
	void logicDisConnect();
	void getter(FifoRead::Data&& data);
	Fifo client;
	FifoRead::ReadHandler readHandler;
};


#endif
