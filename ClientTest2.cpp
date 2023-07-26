
#include <thread>
#include "Fifo/Fifo.h"
#include "Client/Client.h"
#include <iostream>
int main()
{
	using namespace Ipc;
	std::cout << "клиент" << std::endl << std::endl;
	std::string FIFO1 = "/home/ilya/fifo1";
	std::string FIFO2 = "/home/ilya/fifo2";

	int n            = 10;
	std::string data = "";
	data.reserve(n * 1024);

	auto e = [&](FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout << "\n" << std::string(dataq.data(), dataq.data() + dataq.size()) << "\n";
		std::cout << "пришли данные\n";
	};

	// sleep(10);
	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};

	Client client1(k2);
	client1.setReadHandler(e);

	client1.start();
	auto x = 2;
	for(int i = 0; i != 4; i++) {
		std::string a(x, 'a');
		client1.write((void*)a.data(), x);
		sleep(1);
	}
	sleep(2);
	std::cout << "\nstopStart\n";
	client1.stop();

	std::cout << "\nstop\n";

	std::cout << data.size();

	sleep(2);
	return 0;
}
