#include <gtest/gtest.h>

#include "Fifo/Fifo.h"

using namespace std;


TEST(Fifo, 3)
{
	std::string FIFO2 = "fifo2";
	int n             = 10;
	std::string data  = "";
	data.reserve(n * 10);
	auto getterRead = [&](void* dataq, size_t szInBytes) {
		data += std::string((char*)dataq, (char*)dataq + szInBytes);
	};

	Params params = {
	    FIFO2,
	    100,
	    0,
	    getterRead,
	};

	FifoRead client1(params);
	FifoWrite client2(FIFO2);

	client1.startRead();
	client2.startWrite();

	sleep(1);

	client2.stopWrite();
	client1.stopRead();
}
