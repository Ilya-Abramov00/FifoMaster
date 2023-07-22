
#include <thread>
#include "Fifo/Fifo.h"
#include "Server/Server.h"
#include "Client/Client.h"
int main()
{
	std::cout << "клиент" << std::endl << std::endl;

	std::string FIFO3 = "/home/ilya/fifo3";
	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 100;
	std::string data = "";
	data.reserve(n * 1024);

	auto getterRead = [&](FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
	};


	Client client2(FIFO1);
	client2.setReadHandler(getterRead);



	std::string a(n, 'a');

	for(int i = 0; i != 10; i++) {
		client2.write((void*)a.data(), n);
	}

	Client client1(FIFO3);
	client1.setReadHandler(getterRead);


	for(int i = 0; i != 10; i++) {
		client1.write((void*)a.data(), n);
	}
	client1.start();
	client2.start();

	sleep(5);


	client2.stop();
	client1.stop();
	std::cout << data.size();

	return 0;
}
