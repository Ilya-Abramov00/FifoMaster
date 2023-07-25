

#include "Fifo/Fifo.h"
#include "Server/Server.h"

#include <thread>
#include <iostream>
void c(Server::ConnectionsTable w)
{
	std::cout << "CloseConnectionHandler\n";
};
void q(Server::ConnectionsTable w)
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

	auto e = [&](Server::ConnectionsTable, FifoRead::Data&& dataq) {
		data += std::string(dataq.data(), dataq.data() + dataq.size());
		std::cout<<"\n"<<std::string(dataq.data(), dataq.data() + dataq.size())<<"\n";
		std::cout << "пришли данные\n";
	};
FifoCfg a1={FIFO1,FIFO1};
FifoCfg a2={FIFO2,FIFO2};
FifoCfg a3={FIFO3,FIFO3};

	std::list<FifoCfg> z={a1,a2,a3};

	Server a(z);
	a.setReadHandler(e);
	a.setCloseConnectionHandler(c);
	a.setNewConnectionHandler(q);
	a.start();

	auto x=2;
	for(int i = 0; i != 4; i++) {
		std::string eqq(x, 'a');
	a.write(a.connectionTable.at(a1), (void*)eqq.data(), x);
	a.write(a.connectionTable.at(a2), (void*)eqq.data(), x);
	a.write(a.connectionTable.at(a3), (void*)eqq.data(), x);
		sleep(1);
	}




	sleep(11);

	sleep(10);

	std::cout<<"\nstopStart\n";
	a.stop();
	std::cout<<"\nstop\n";
	std::cout << data.size();
	return 1;
}