
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

	auto e = [&](FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout << "\n" << std::string(dataq.data(), dataq.data() + dataq.size()) << "\n";
		std::cout << "пришли данные\n";
	};

	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};

	Client client2(k2, Ipc::Config::QW);
	client2.setReadHandler(e);
	client2.setDisconnectHandler([]() {
	});
	client2.setConnectHandler([]() {
	});

	client2.start();
	auto x = 2;
	for(int i = 0; i != 4; i++) {
		std::string a("2 канал ");
		client2.write((void*)a.data(), a.size());
		sleep(1);
	}
	sleep(2);
	std::cout << "\nstop\n";
	client2.stop();

	std::cout << "\nStart\n";
	client2.start();

	for(int i = 0; i != 4; i++) {
		std::string a("2 канал");
		client2.write((void*)a.data(), a.size());
		sleep(1);
	}
	std::cout << "\nstop\n";
	client2.stop();
	sleep(5);



	std::cout <<"пришло "<< data.size();
	std::cout << "\nдолжно быть "<<30;
	sleep(2);
	return 0;
}
