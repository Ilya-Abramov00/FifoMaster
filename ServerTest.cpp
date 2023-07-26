


#include "Fifo/Fifo.h"
#include "Server/Server.h"

#include <thread>
#include <iostream>
using namespace Ipc;

int main()
{
	std::cout << "Server\n\n";
	std::string FIFO3 = "/home/ilya/fifo3";
	std::string FIFO2 = "/home/ilya/fifo2";
	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 10;
	std::string data = "";
	data.reserve(n * 1024);

	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};
	FifoCfg k3{FIFO3, FIFO3 + "_reverse"};

	auto getter = [&](size_t id, FifoRead::Data&& dataq) {
		data.insert(data.begin(), dataq.data(), dataq.data() + dataq.size());
		std::cout<<"данные пришли";
	};

	Server server({k1, k2, k3},Ipc::Config::QW);
	server.setReadHandler(getter);

	server.setConnectHandler([](size_t) {
	});
	server.setDisconnectHandler([](size_t) {
	});

	server.start();

	auto x = 2;
	std::string data0(x, 'a');
	for(int i = 0; i != 4; i++) {
		server.write(0, (void*)data0.data(), x);
		server.write(1, (void*)data0.data(), x);
		server.write(2, (void*)data0.data(), x);
		sleep(1);
	}
	std::string data1(x, 'v');
	for(int i = 0; i != 4; i++) {
		server.write(0, (void*)data0.data(), x);
		server.write(1, (void*)data0.data(), x);
		server.write(2, (void*)data0.data(), x);
		sleep(1);
	}

	sleep(15);

	std::cout << "\nstopStart\n";
	server.stop();
	std::cout << "\nstop\n";
	std::cout << data.size();
	return 1;
}