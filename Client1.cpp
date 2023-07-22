
#include <thread>
#include "Fifo/Fifo.h"
#include "Server/Server.h"
#include "Client/Client.h"
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


	Client client2(FIFO1);
	client2.setReadHandler(e);

	Client client1(FIFO3);
	client1.setReadHandler(e);

	client1.start();
	sleep(3);
	client2.start();

auto x=2;
	for(int i = 0; i != 4; i++) {
		std::string a(x, 'a');
		client2.write((void*)a.data(),x);
		std::string z(x, 'z');
		client1.write((void*)z.data(), x);
		sleep(1);
	}
	sleep(7);


	std::cout<<"\nstop\n";
	client1.stop();
	client2.stop();
	std::cout << data.size();
	sleep(6);
	return 0;
}
