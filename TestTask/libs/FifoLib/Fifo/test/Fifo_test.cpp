#include <gtest/gtest.h>

#include "Fifo/Fifo.h"

using namespace std;

TEST(Fifo, empty)
{
	std::string FIFO2 = "fifo2";
	int n             = 10;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	auto connect = []() {

	};
	Fifo client1(FIFO2,FIFO2);

	client1.setReadHandler(getterRead);

	client1.setConnectionHandlerRead(connect);
	client1.setConnectionHandlerWrite(connect);

	client1.setDisConnectionHandlerRead(connect);
	client1.setDisConnectionHandlerWrite(connect);

	client1.start();

	sleep(1);

	client1.stopWrite();
	client1.stopRead();
}

TEST(Fifo, null_ptr)
{
	std::string FIFO2 = "fifo2";
	int n             = 10;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	auto connect = []() {

	};
	Fifo client1(FIFO2,FIFO2);

	client1.setReadHandler(getterRead);

	client1.setConnectionHandlerRead(connect);
	client1.setConnectionHandlerWrite(connect);

	client1.setDisConnectionHandlerRead(connect);
	client1.setDisConnectionHandlerWrite(connect);

	client1.start();

	void* a = NULL;
	client1.write(a, 2);

	sleep(1);

	client1.stopWrite();
	client1.stopRead();
	ASSERT_TRUE(data.size() == 0);
}

TEST(Fifo, time)
{
	std::string FIFO2 = "fifo2";
	int n             = 1024 * 5;
	std::string data  = "";
	data.reserve(n);
	auto getterRead = [&](FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	auto connect = []() {

	};
	Fifo client1(FIFO2,FIFO2);

	client1.setReadHandler(getterRead);

	client1.setConnectionHandlerRead(connect);
	client1.setConnectionHandlerWrite(connect);

	client1.setDisConnectionHandlerRead(connect);
	client1.setDisConnectionHandlerWrite(connect);

	client1.start();

	std::string da(n, '3');

	client1.write((void*)da.data(), n);

	sleep(1);


	client1.stopWrite();
	client1.stopRead();
	ASSERT_TRUE(data.size() == n);
	ASSERT_TRUE(data == std::string(n, '3'));
}
