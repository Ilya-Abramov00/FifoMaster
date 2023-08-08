
#include <thread>
#include "Fifo/Fifo.h"
#include "Client/Client.h"
#include <iostream>
int main()
{
	using namespace Ipc;
	std::cout << "клиент" << std::endl << std::endl;

	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 10;
	std::string data = "";
	data.reserve(n * 1024);
	std::mutex mtx;
	auto e = [&mtx, &data](FifoRead::Data&& dataq) {
		std::lock_guard<std::mutex> mtx0(mtx);
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout << "пришли данные ";
		std::cout << std::string(dataq.data(), dataq.data() + dataq.size()) << "\n";
	};

	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};

	Client client1(k1, Ipc::Config::QW);
	client1.setReadHandler(e);
	client1.setDisconnectHandler([]() {
	});
	client1.setConnectHandler([]() {
	});

	client1.start();

	auto x = 1;
	for(int i = 0; i != 10; i++) {
		std::string z("1 канал");
		client1.write((void*)z.data(), z.size());
		sleep(1);
	}
	sleep(8);

	std::cout << "\nstop\n";
	client1.stop();

	sleep(1);

	client1.start();

	std::cout << "\n2 start\n";
	for(int i = 0; i != 5; i++) {
		std::string z("1 канал");
		client1.write((void*)z.data(), z.size());
		sleep(1);
	}
	sleep(12);

	std::cout << "\n2 stop\n";
	client1.stop();
	std::cout <<"пришло "<< data.size();
	std::cout << "\nдолжно быть "<<30;

	return 0;
}
