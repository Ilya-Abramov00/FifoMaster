#include <gtest/gtest.h>

#include "Fifo/Fifo.h"

using namespace std;

TEST(Fifo, empty)
{
	std::string FIFO2 = "fifo2";
	int n             = 10;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](Data && dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	auto connect = []() {

	};
	Params params = {
	    FIFO2,
	    getterRead,
	    connect
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
	auto getterRead = [&](Data && dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	auto connect = []() {

	};
	Params params = {
	    FIFO2,
	    getterRead,
	    connect
	};

	FifoRead client1(params);
	FifoWrite client2(FIFO2);

	client1.startRead();
	client2.startWrite();

	void* a = NULL;
	client2.pushData(a, 0);

	sleep(1);

	client2.stopWrite();
	client1.stopRead();
}

TEST(Fifo, time)
{
	std::string FIFO2 = "fifo2";
	int n             = 1024*5;
	std::string data  = "";
	data.reserve(n );
	auto getterRead = [&](Data && dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	auto connect = []() {

	};
	Params params = {
	    FIFO2,
	    getterRead,
	    connect
	};

	FifoRead client1(params);
	FifoWrite client2(FIFO2);

	client1.startRead();
	client2.startWrite();

	std::string da(n, '3');
	client2.pushData(da.data(), n);


	sleep(1);

	client2.stopWrite();
	client1.stopRead();
	ASSERT_TRUE(data.size() == n );
	ASSERT_TRUE(data == std::string(n , '3'));
}
