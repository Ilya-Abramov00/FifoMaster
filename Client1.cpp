
#include <thread>
#include "Fifo/Fifo.h"

int main()
{
	std::cout << "клиент" << std::endl << std::endl;

	std::string FIFO2 = "/home/ilya/fifo2";
	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 100;
	std::string data = "";
	data.reserve(n * 1024);

	auto getterRead = [&](FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};
	auto connect = []() {
		std::cout << "произошел logicConnect" << std::endl;
	};
	auto disconnect = []() {
		std::cout << "произошел disconnect" << std::endl;
	};

	Fifo client1(FIFO1 + "_reverse", FIFO1);
	client1.setReadHandler(getterRead);
	client1.setConnectionHandlerRead(connect);
	client1.setDisConnectionHandlerRead(disconnect);
	std::string a(n, 'a');

	for(int i = 0; i != 10; i++) {
		client1.write((void*)a.data(), n);
	}
	client1.start();
	sleep(15);
	client1.stop();

	std::cout << data.size();

	return 0;
}
