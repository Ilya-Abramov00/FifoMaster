
#include <thread>
#include "Fifo/Fifo.h"
#include "Client/Client.h"
#include <iostream>
int main()
{
	using namespace Ipc;
	std::cout << "клиент" << std::endl << std::endl;

	std::string FIFO2 = "/home/ilya/fifo2";
	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 10;
	std::string data = "";
	data.reserve(n * 1024);
std::mutex mtx;
	auto e = [&mtx,&data](FifoRead::Data&& dataq) {
	std::lock_guard<std::mutex>mtx0(mtx);
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout << "пришли данные ";
		std::cout << std::string(dataq.data(), dataq.data() + dataq.size()) << "\n";
	};

	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};

	Client client2(k1, Ipc::Config::QW);
	client2.setReadHandler(e);
	client2.setDisconnectHandler([]() {
	});
	client2.setConnectHandler([]() {
	});

	Client client1(k2, Ipc::Config::QW);
	client1.setReadHandler(e);
	client1.setDisconnectHandler([]() {
	});
	client1.setConnectHandler([]() {
	});

	client1.start();
	client2.start();

	auto x = 1;
	for(int i = 0; i != 5; i++) {
		std::string a(x, 'a');
		client2.write((void*)a.data(), x);
		std::string z(x, 'z');
		client1.write((void*)z.data(), x);
		sleep(1);
	}
	sleep(8);

	std::cout << "\nstop\n";
	client1.stop();
	client2.stop();
	std::cout << data.size();


	client1.start();
	client2.start();
	std::cout << "\nstart\n";
	for(int i = 0; i != 5; i++) {
		std::string a(x, 'a');
		client2.write((void*)a.data(), x);
		std::string z(x, 'z');
		client1.write((void*)z.data(), x);
		sleep(1);
	}
	sleep(8);

	std::cout << "\nstop\n";
	client1.stop();
	client2.stop();
	return 0;
}
