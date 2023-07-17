#include <gtest/gtest.h>

#include "Fifo/Fifo.h"

using namespace std;

TEST(Fifo, empty)
{
	std::string FIFO2 = "fifo2";
	int n             = 10;
	std::string data  = "";
	data.reserve(n * 12);
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

TEST(Fifo, null_ptr)
{
	std::string FIFO2 = "fifo2";
	int n             = 10;
	std::string data  = "";
	data.reserve(n * 12);
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

	void * a=NULL;
	client2.pushData(a,0);

	sleep(1);

	client2.stopWrite();
	client1.stopRead();
}



TEST(Fifo, time)
{
	std::string FIFO2 = "fifo2";
	int n             = 10;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](void* dataq, size_t szInBytes) {
		data += std::string((char*)dataq, (char*)dataq + szInBytes);
	};

	Params params = {
	    FIFO2,
	    100,
	    50,
	    getterRead,
	};

	FifoRead client1(params);
	FifoWrite client2(FIFO2);

	client1.startRead();
	client2.startWrite();

	for(int i=0;i!=10;i++){
		std::string da(n,'3');
		client2.pushData(da.data(),n);
	}

	sleep(1);

	client2.stopWrite();
	client1.stopRead();
	ASSERT_TRUE(data.size() == n*10);
	ASSERT_TRUE(data == std::string(n*10,'3'));
}
