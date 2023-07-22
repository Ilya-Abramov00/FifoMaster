#include <gtest/gtest.h>

#include "Server/Server.h"

using namespace std;
void c(Server::ConnectionId w)
{
	std::cout << "CloseConnectionHandler\n";
};
void q(Server::ConnectionId w)
{
	std::cout << "NewConnectionHandler\n";
};
TEST(Fifo, start)
{
	std::string FIFO3 = "/home/ilya/fifo3";
	std::string FIFO2 = "/home/ilya/fifo2";
	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 100;
	std::string data = "";
	data.reserve(n * 1024);

	auto e = [&](Server::ConnectionId, FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout << "пришли данные\n";
	};

	Server a(std::vector<std::string>{FIFO1,FIFO2,FIFO3});

	a.setReadHandler(e);
	a.setCloseConnectionHandler(c);
	a.setNewConnectionHandler(q);

	std::string eqq(n, 'a');

	a.write(a.connectionId.at(FIFO1), (void*)eqq.data(), n);//надо  уметь сигнализировать о том, что данные не отправились

	a.start();

	sleep(1);

	a.stop();

	a.start();

	sleep(1);

	a.stop();
}
TEST(Fifo, empty)
{
	std::string FIFO3 = "/home/ilya/fifo3";
	std::string FIFO2 = "/home/ilya/fifo2";
	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 100;
	std::string data = "";
	data.reserve(n * 1024);

	auto e = [&](Server::ConnectionId, FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout << "пришли данные\n";
	};

	Server a(std::vector<std::string>{FIFO1,FIFO2,FIFO3});

	a.setReadHandler(e);
	a.setCloseConnectionHandler(c);
	a.setNewConnectionHandler(q);

	std::string eqq(n, 'a');

	a.write(a.connectionId.at(FIFO1), (void*)eqq.data(), n);//надо  уметь сигнализировать о том, что данные не отправились

	a.start();

	sleep(1);



	a.stop();


}

