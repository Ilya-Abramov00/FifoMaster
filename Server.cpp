

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
std::cout<<"Server\n\n";
	std::string FIFO3 = "/home/ilya/fifo3";
	std::string FIFO2 = "/home/ilya/fifo2";
	std::string FIFO1 = "/home/ilya/fifo1";

	int n            = 10;
	std::string data = "";
	data.reserve(n * 1024);

	auto e = [&](Server::ConnectionId, FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout<<"\n"<<std::string(dataq.data(), dataq.data() + dataq.size())<<"\n";
		std::cout << "пришли данные\n";
	};

	Server a(std::vector<std::string>{FIFO1,FIFO2,FIFO3});

	a.setReadHandler(e);
	a.setCloseConnectionHandler(c);
	a.setNewConnectionHandler(q);
	a.start();

	auto x=2;
	for(int i = 0; i != 4; i++) {
		std::string eqq(x, 'a');
	a.write(a.connectionId.at(FIFO1), (void*)eqq.data(), x);
	a.write(a.connectionId.at(FIFO2), (void*)eqq.data(), x);
	a.write(a.connectionId.at(FIFO3), (void*)eqq.data(), x);
		sleep(1);
	}

	sleep(7);


	std::cout<<"\nstop\n";
	a.stop();

	std::cout << data.size();
	return 1;
}