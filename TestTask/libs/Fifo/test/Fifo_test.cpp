#include <gtest/gtest.h>

#include "Fifo/Fifo.h"
#include <thread>
#include <list>
using namespace std;

// TEST(FifoRead, read)
//{
//	std::string FIFO1 = "fifo1";
//	std::string FIFO2 = "fifo2";
//
//	FifoRead client2(FIFO2);
//	client2.start_read();
//
//	std::string data = "";
//
//	client2.readFifo(data, 20);
// }
// TEST(FifoWrite, wride)
//{
//	std::string FIFO1 = "fifo1";
//	std::string FIFO2 = "fifo2";
//
//	std::list<string> ret = {"adwd", "defwefwef", "cdsacadcsd", "", ""};
//	auto getter           = [&]() {
//         sleep(1);
//         ret.pop_front();
//
//         return *ret.begin();
//	};
//
//	FifoWrite client2(FIFO2);
//	client2.setMsgGetter(getter);
//	client2.start_write();
//
//	client2.writeFifo();
// }


TEST(Fifo, 1)
{

	std::string FIFO2 = "fifo2";

	FifoWrite client2(FIFO2);
	FifoRead client1(FIFO2);

int i=0;
	auto getter           = [&]() {
	++i;
		return  std::string(100, '*');
	};

	client2.setMsgGetter(getter);
	client2.start_write();

	std::thread t1([&]() {
		client2.writeFifo();
	});

	client1.start_read();

	std::string data = "";
	std::thread t2([&]() {
		client1.readFifo(data, 128);
	});


	sleep(2.5);
	client2.stop_write();
	sleep(0.5);
	client1.stop_read();
	std::cout<<data.size()/100<<endl;
	std::cout<<i<<endl;
	ASSERT_TRUE(data.size()/100==i);

	t1.join();
	t2.join();
}