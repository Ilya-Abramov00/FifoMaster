#include "gtest/gtest.h"

#include "Fifo/Fifo.h"

using namespace std;
using namespace Ipc;
class WriterFactory {
public:
	static std::unique_ptr<IFifoWriter> create(std::string filename, Config conf,size_t time)
	{
		switch(conf) {
		case(Config::QW):
			return std::unique_ptr<IFifoWriter>(new WriteQImpl(filename));
		case(Config::NQW):
			return std::unique_ptr<IFifoWriter>(new WriteDirectImpl(filename,time));
		}
	}
};
TEST(Fifo, empty)
{
	std::string FIFO2 = "fifo2";
	int n             = 10;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](Ipc::FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	int flagConnect    = 0;
	int flagDisconnect = 0;
	auto connect       = [&flagConnect]() {
        flagConnect++;
	};

	auto disconnect = [&flagDisconnect]() {
		flagDisconnect++;
	};
	Fifo client1(WriterFactory::create(FIFO2, Ipc::Config::NQW,2000), FIFO2);

	client1.setReadHandler(getterRead);

	client1.setConnectionHandlerRead(connect);
	client1.setConnectionHandlerWrite(connect);

	client1.setDisconnectionHandlerRead(disconnect);
	client1.setDisconnectionHandlerWrite(disconnect);

	client1.start();
	std::string a("wqrtt");
	client1.write(a.c_str(), a.size());

	sleep(1);

	client1.stop();
	sleep(1);

	ASSERT_TRUE(a == data);
	ASSERT_TRUE(flagConnect == 2);
	ASSERT_TRUE(flagDisconnect == 2);
}

TEST(Fifo, empty1)
{
	std::string FIFO2 = "fifo2";
	int n             = 10;
	std::string data  = "";
	data.reserve(n * 12);
	auto getterRead = [&](Ipc::FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	int flagConnect    = 0;
	int flagDisconnect = 0;
	auto connect       = [&flagConnect]() {
        flagConnect++;
	};

	auto disconnect = [&flagDisconnect]() {
		flagDisconnect++;
	};
	Fifo client1(WriterFactory::create(FIFO2, Ipc::Config::QW,0), FIFO2);

	client1.setReadHandler(getterRead);

	client1.setConnectionHandlerRead(connect);
	client1.setConnectionHandlerWrite(connect);

	client1.setDisconnectionHandlerRead(disconnect);
	client1.setDisconnectionHandlerWrite(disconnect);

	client1.start();
	std::string a("ewg");
	client1.write(a.c_str(), a.size());

	sleep(1);

	client1.stop();
	sleep(1);

	ASSERT_TRUE(flagConnect == 2);
	ASSERT_TRUE(flagDisconnect == 2);
}
