
#include "gtest/gtest.h"

#include "Fifo/Fifo.h"

TEST(big_data_Q, 1)
{
	using namespace std;
	using namespace Ipc;

	std::string FIFO2 = "fifo2";
	int n             = 1024 * 1024;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](FifoRead::Data&& dataq) {
		data.insert(data.end(), dataq.data(), dataq.data() + dataq.size());
	};

	FifoRead client1(FIFO2);

	WriteQImpl client2(FIFO2);

	int flagConnect    = 0;
	int flagDisconnect = 0;

	auto connect = [&flagConnect]() {
		flagConnect++;
	};

	auto disconnect = [&flagDisconnect]() {
		flagDisconnect++;
	};
	client1.setReadHandler(getterRead);

	client1.setConnectionHandler(connect);
	client2.setConnectionHandler(connect);

	client1.setDisconnectionHandler(disconnect);
	client2.setDisconnectionHandler(disconnect);

	client2.startWrite();
	client1.startRead();

	std::string a(n, '1');

	client2.pushData((void*)a.data(), n);

	std::string b(n, '2');

	client2.pushData((void*)b.data(), n);

	std::string c(n, '3');

	client2.pushData((void*)c.data(), n);

	std::string d(n, '4');

	client2.pushData((void*)d.data(), n);

	std::string e(n, '5');

	client2.pushData((void*)e.data(), n);

	std::string f(n, '6');

	client2.pushData((void*)f.data(), n);

	std::string t7(n, '7');

	client2.pushData((void*)t7.data(), n);

	std::string t8(n, '8');

	client2.pushData((void*)t8.data(), n);

	std::string t9(n, '9');

	client2.pushData((void*)t9.data(), n);

	sleep(3);

	client2.stopWrite();

	client1.stopRead();

	sleep(1);

	a += b;
	a += c;
	a += d;
	a += e;
	a += f;
	a += t7;
	a += t8;
	a += t9;

	ASSERT_TRUE(flagConnect == 2);
	ASSERT_TRUE(flagDisconnect == 2);

	ASSERT_TRUE(data.size() == n * 9);
	ASSERT_TRUE(data == a);
}
TEST(big_data_Q, 2)
{
	using namespace std;
	using namespace Ipc;

	std::string FIFO2 = "fifo2";
	int n             = 1024 * 1024 * 10;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](FifoRead::Data&& dataq) {
		data.insert(data.end(), dataq.data(), dataq.data() + dataq.size());
	};
	auto connect = []() {

	};
	FifoRead client1(FIFO2);
	WriteQImpl client2(FIFO2);

	client1.setReadHandler(getterRead);

	client1.setConnectionHandler(connect);
	client1.setDisconnectionHandler(connect);

	client2.setConnectionHandler(connect);
	client2.setDisconnectionHandler(connect);

	client1.startRead();
	client2.startWrite();

	std::string a(n, '1');

	client2.pushData((void*)a.data(), n);

	std::string b(n, '2');

	client2.pushData((void*)b.data(), n);

	std::string c(n, '3');

	client2.pushData((void*)c.data(), n);

	std::string d(n, '4');

	client2.pushData((void*)d.data(), n);

	std::string e(n, '5');

	client2.pushData((void*)e.data(), n);

	std::string f(n, '6');

	client2.pushData((void*)f.data(), n);

	std::string t7(n, '7');

	client2.pushData((void*)t7.data(), n);

	std::string t8(n, '8');

	client2.pushData((void*)t8.data(), n);

	std::string t9(n, '9');

	client2.pushData((void*)t9.data(), n);

	std::string t10(n, '0');

	client2.pushData((void*)t10.data(), n);

	sleep(3);

	client2.stopWrite();
	client1.stopRead();

	a += b;
	a += c;
	a += d;
	a += e;
	a += f;
	a += t7;
	a += t8;
	a += t9;
	a += t10;

	ASSERT_TRUE(data.size() == n * 10);
	ASSERT_TRUE(data == a);
}
