
#include <thread>
#include "Fifo/Fifo.h"
#include "Server/Server.h"
#include "Client/Client.h"
int main()
{
	std::cout << "клиент" << std::endl << std::endl;

	std::string FIFO2 = "/home/ilya/fifo2";

	int n            = 10;
	std::string data = "";
	data.reserve(n * 1024);

	auto e = [&]( FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout<<"\n"<<std::string(dataq.data(), dataq.data() + dataq.size())<<"\n";
		std::cout << "пришли данные\n";
	};



	Client client1(FIFO2);
	client1.setReadHandler(e);


	client1.start();
	auto x=2;
	for(int i = 0; i != 4; i++) {
		std::string a(x, 'a');
		client1.write((void*)a.data(), x);
		sleep(1);
	}
	sleep(7);
	std::cout<<"\nstop\n";
	client1.stop();



	std::cout << data.size();

	sleep(15);
	return 0;
}
