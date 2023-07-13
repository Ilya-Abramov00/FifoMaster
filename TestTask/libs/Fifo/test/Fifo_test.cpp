#include <gtest/gtest.h>

#include "Fifo/Fifo.h"

using namespace std;

// TEST(FifoRead, read)
//{
//	std::string FIFO1 = "fifo1";
//	std::string FIFO2 = "fifo2";
//
//	FifoRead client2(FIFO2);
//	client2.startRead();
//
//	std::string data = "";
//
//	client2.readFifo(data, 20);
// }
// TEST(FifoWrite, write)
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
//	client2.startWrite();
//
//	client2.writeFifo();
// }

TEST(Fifo, 1)
{
/*
	std::string FIFO2 = "/home/ilya/Fifo/fifo2";

	std::string data = "";

	auto getterRead  = [&](const char* dataq, size_t szInBytes) {
 //      std::cout << std::string(dataq);
        data += dataq;
	};

	Params params = {
	    FIFO2,
	    111,
	    0,
	    getterRead,
	};

	std::mutex mtx;

	FifoRead client1(params);

	FifoWrite client2(FIFO2, mtx);

	int i       = 0;
	int Nq=56;
	auto getter = [&i,&Nq]() {
		i+=Nq;
		return std::pair(std::string(Nq, '*'), Nq);
	};

	client2.setMsgGetter(getter);
sleep(0.1);
	client1.startRead(data);
	sleep(0.1);
	client2.startWrite();

	sleep(0.1);
	client2.writeUser();


	sleep(3);

	client2.stopWrite();

	sleep(2);
	client1.stopRead();

	std::cout << "\n\nсчиталось  " << data.size()  << endl;
	std::cout << i << endl;
	ASSERT_TRUE(data.size()  == i);
*/

}

// TEST(Fifo, 2)
//{
//	std::string FIFO2 = "fifo2";
//
//	FifoWrite client2(FIFO2);
//	FifoRead client1(FIFO2);
//
//	int i       = 0;
//	auto getter = [&]() {
//		++i;
//		return std::string(100, '*');
//	};
//
//	client2.setMsgGetter(getter);
//	client2.startWrite();
//
//	std::thread t1([&]() {
//		client2.writeFifo();
//	});
//
//	client1.startRead();
//
//	std::string data = "";
//	std::thread t2([&]() {
//		client1.readFifo(data, 17); // лучше использовать кратные 8
//	});
//
//	sleep(2);
//	client2.stopWrite();
//	sleep(4);
//	client1.stopRead();
//	std::cout << data.size() / 100 << endl;
//	std::cout << i << endl;
//	ASSERT_TRUE(data.size() / 100 == i);
//
//	t1.join();
//	t2.join();
// }