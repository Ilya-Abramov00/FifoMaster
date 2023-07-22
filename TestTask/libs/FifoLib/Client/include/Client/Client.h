#ifndef Client_H
#define Client_H


#include "Fifo/Fifo.h"

class Client {
public:

	Client(std::string nameChannel);
void start();
void stop();
void write(const void* data, size_t sizeN);
void setReadHandler(FifoRead::ReadHandler h);
private:
	void logicConnect();
	void logicDisConnect();
	void getter(FifoRead::Data&& data);
	Fifo client;
	FifoRead::ReadHandler readHandler;
};


#endif
