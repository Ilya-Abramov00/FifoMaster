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

	client1.setDisconnectionHandlerRead(connect);
	client1.setDisconnectionHandlerWrite(connect);

	client1.start();

	sleep(1);

		client1.stop();

		ASSERT_TRUE(data.size() == sizeN);
	}
	class WriterFactory {
	public:
		static std::unique_ptr<IFifoWriter> create(std::string filename, Config conf)
		{
			switch(conf) {
			case(Config::QW):
				return std::unique_ptr<IFifoWriter>(new QWriteImpl(filename));
			case(Config::NQW):
				return std::unique_ptr<IFifoWriter>(new NQWriteImpl(filename));
			}
		}
	};

TEST(Fifo, null_ptr)
{
	using namespace std;
	using namespace Ipc;

	FifoT(FIFO2, FIFO2, Ipc::Config::NQW, nullptr, 0);
}

TEST_F(FifoTest, time)
{
	using namespace std;
	using namespace Ipc;

	int n = 1024;
	std::string da(n, '3');
	FifoT(FIFO2, FIFO2, Ipc::Config::NQW, da.data(), 1024);
}
