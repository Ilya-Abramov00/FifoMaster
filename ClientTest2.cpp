
#include <thread>
#include "Fifo/Fifo.h"
#include "Client/Client.h"
#include <iostream>
int main()
{
	using namespace Ipc;
	std::cout << "клиент" << std::endl << std::endl;

	std::string FIFO2 = "/home/ilya/fifo2";

	int n            = 10;
	std::string data = "";
	data.reserve(n * 1024);

	auto e = [&data](FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout << std::string(dataq.data(), dataq.data() + dataq.size()) << "\n";
		std::cout << "пришли данные\n";
	};

	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};

	Client client2(k2, Ipc::Config::NQW, 8000);
	client2.setReadHandler(e);
	client2.setCloseHandler([]() {
	});
	client2.setNewHandler([]() {
	});

	client2.start();
	std::cout << "\nStart\n";
	for(int i = 0; i != 15; i++) {
		std::string a("2 channel ");
		client2.write((void*)a.data(), a.size());
		if(i % 3) {
			sleep(1);
		}
	}
	sleep(2);
	std::cout << "\nstop\n";
	client2.stop();

	sleep(7);

	std::cout << "\nStart\n";
	client2.start();

	for(int i = 0; i != 15; i++) {
		std::string a("2 channel ");
		client2.write((void*)a.data(), a.size());
		if(i % 3) {
			sleep(1);
		}
	}
	std::cout << "\nstop\n";
	sleep(10);
	client2.stop();

	std::cout << "пришло " << data.size();
	sleep(2);
	return 0;
}
