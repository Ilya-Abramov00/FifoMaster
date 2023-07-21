
#include <thread>
#include "Fifo/Fifo.h"

int main()
{
	std::cout << "клиент" << std::endl << std::endl;

	std::string FIFO3 = "/home/ilya/fifo3";
	std::string FIFO2 = "/home/ilya/fifo2";
	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 100;
	std::string data = "";
	data.reserve(n * 1024);

	auto getterRead = [&](FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	auto connectR = []() {
		std::cout << "произошел logicConnectRead" << std::endl;
	};
	auto connectW = []() {
		std::cout << "произошел logicConnectWrite" << std::endl;
	};
	auto disconnectR = []() {
		std::cout << "произошел disconnectRead" << std::endl;
	};
	auto disconnectW = []() {
		std::cout << "произошел disconnectWrite" << std::endl;
	};

	Fifo client2(FIFO1 + "_reverse", FIFO1);
	client2.setReadHandler(getterRead);

	client2.setConnectionHandlerRead(connectR);
	client2.setDisConnectionHandlerRead(disconnectR);

	client2.setConnectionHandlerWrite(connectW);
	client2.setDisConnectionHandlerWrite(disconnectW);

	std::string a(n, 'a');

	for(int i = 0; i != 10; i++) {
		client2.write((void*)a.data(), n);
	}

	Fifo client1(FIFO1, FIFO1 + "_reverse");
	client1.setReadHandler(getterRead);

	client1.setConnectionHandlerRead(connectR);
	client1.setDisConnectionHandlerRead(disconnectR);

	client1.setConnectionHandlerWrite(connectW);
	client1.setDisConnectionHandlerWrite(disconnectW);

	for(int i = 0; i != 10; i++) {
		client1.write((void*)a.data(), n);
	}
	client1.start();
	client2.start();

	sleep(2);

	std::thread t([&client2]() {
		client2.stop();
	});

	client1.stop();
	std::cout << data.size();
	t.join();
	return 0;
}
