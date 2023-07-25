
#include <thread>
#include "Fifo/Fifo.h"
#include "Client/Client.h"
#include <iostream>
int main()
{
	std::cout << "клиент" << std::endl << std::endl;

	std::string FIFO3 = "/home/ilya/fifo3";
	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 10;
	std::string data = "";
	data.reserve(n * 1024);

	auto e = [&]( FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout<<"\n"<<std::string(dataq.data(), dataq.data() + dataq.size())<<"\n";
		std::cout << "пришли данные\n";
	};

FifoCfg k1{FIFO1,FIFO1+"_reverse"};
FifoCfg k3{FIFO3,FIFO3+"_reverse"};

	Client client2(k1);
	client2.setReadHandler(e);

	Client client1(k3);
	client1.setReadHandler(e);

	client1.start();
	client2.start();

auto x=2;
	for(int i = 0; i != 4; i++) {
		std::string a(x, 'a');
		client2.write((void*)a.data(),x);
		std::string z(x, 'z');
		client1.write((void*)z.data(), x);
		sleep(1);
	}
	sleep(9);


	std::cout<<"\nstop\n";
	client1.stop();
	client2.stop();
	sleep(9);
	std::cout << data.size();
	sleep(2);
	return 0;
}
