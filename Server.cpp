

#include "Fifo/Fifo.h"
#include "Server/Server.h"

#include <thread>

void c(Server::ConnectionId w)
{
	std::cout << "CloseConnectionHandler\n";
};
void q(Server::ConnectionId w)
{
	std::cout << "NewConnectionHandler\n";
};
int main()
{
	//	std::cout << "клиент" << std::endl << std::endl;
	//
	std::string FIFO3 = "/home/ilya/fifo3";
	std::string FIFO2 = "/home/ilya/fifo2";
	std::string FIFO1 = "/home/ilya/fifo1";
	//	int n             = 100;
	//	std::string data  = "";
	//	data.reserve(n * 1024);
	//
	//	auto getterRead = [&](Data&& dataq) {
	//		data += std::string(dataq.data(), dataq.data() + dataq.size());
	//	};
	//	auto logicConnect = []() {
	//		std::cout << "произошел коннект" << std::endl;
	//	};
	//
	//	Fifo client1(FIFO2, FIFO1);
	//	client1.setReadHandler(getterRead);
	//	client1.setConnectionHandlerRead(logicConnect);
	//	client1.start();
	//
	//	std::string a(n, 'a');
	//
	//	for(int i = 0; i != 10; i++) {
	//		client1.write((void*)a.data(), n);
	//	}
	//	sleep(5);
	//	client1.stop();
	//
	//	std::cout << "клиент завершил отправку" << std::endl;
	//

	//	std::cout << data.size();
	int n            = 100;
	std::string data = "";
	data.reserve(n * 1024);

	auto e = [&](Server::ConnectionId, FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout << "пришли данные\n";
	};

	Server a(std::vector<std::string>{FIFO1});

	a.setReadHandler(e);
	a.setCloseConnectionHandler(c);
	a.setNewConnectionHandler(q);

	std::string eqq(n, 'a');

	a.write(a.connectionId.at(FIFO1), (void*)eqq.data(), n);

	a.start();

	sleep(5);

	a.stop();

	std::cout << data.size();
	return 1;
}