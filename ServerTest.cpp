


#include "Fifo/Fifo.h"
#include "Server/Server.h"

#include <thread>
#include <iostream>
using namespace Ipc;

int main()
{
	std::cout << "Server\n\n";
//указан прямой путь т.к. собирают main в разные папки
	std::string FIFO1 = "/home/ilya/fifo1";
	std::string FIFO2 = "/home/ilya/fifo2";

	int n            = 10;
	std::string data = "";
	data.reserve(n * 1024);

	FifoCfg k1{FIFO1, FIFO1 + "_reverse"};
	FifoCfg k2{FIFO2, FIFO2 + "_reverse"};

	std::mutex mtx;
	auto getter = [&mtx,&data](size_t id, FifoRead::Data&& dataq) {
		std::lock_guard<std::mutex>mtx0(mtx);
		data.insert(data.begin(), dataq.data(), dataq.data() + dataq.size());
		std::cout<<"данные пришли ";
		std::cout<<std::string(dataq.data(), dataq.data() + dataq.size())<<"\n";
	};

	Server server({k1, k2});
	server.setReadHandler(getter);

	server.setConnectHandler([](size_t) {
	});
	server.setDisconnectHandler([](size_t) {
	});
	size_t id = 2;
	server.setIdDistributionHandler([&id]() {
		return id++;
	});

	server.start();
	std::cout << "\nStart\n";

	auto x = 1;
	std::string data0(x, 'a');
	for(int i = 0; i != 15; i++) {
		server.write(2, (void*)data0.data(), x);
		server.write(3, (void*)data0.data(), x);
		if(i % 3) {
			sleep(1);
		}
	}


	sleep(6);
	server.disconnect(3);
	for(int i = 0; i != 15; i++) {
		server.write(2, (void*)data0.data(), x);
		server.write(3, (void*)data0.data(), x);
		if(i % 3) {
			sleep(1);
		}
	}
	sleep(6);


	server.stop();
sleep(2);
	return 1;
}