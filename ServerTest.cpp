

#include "Fifo/Fifo.h"
#include "Server/Server.h"

#include <thread>
#include <iostream>
void c(FifoCfg w)
{
	std::cout << "CloseConnectionHandler\n";
};
void q(FifoCfg w)
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


	FifoCfg k1{FIFO1,FIFO1+"_reverse"};
	FifoCfg k2{FIFO2,FIFO2+"_reverse"};
	FifoCfg k3{FIFO3,FIFO3+"_reverse"};

	auto e = [&](FifoCfg name, FifoRead::Data&& dataq) {

	  std::cout << "\n пришли от "<<name.directFile;
	  data += std::string(dataq.data(), dataq.data() + dataq.size());
	  std::cout<<"\n"<<std::string(dataq.data(), dataq.data() + dataq.size())<<"\n";
	};
	std::list<FifoCfg> z={k1,k2,k3};

	Server a(z);
	a.setReadHandler(e);
	a.setCloseConnectionHandler(c);
	a.setNewConnectionHandler(q);
	a.start();

	auto x=2;
	for(int i = 0; i != 4; i++) {
		std::string eqq(x, 'a');
	a.write(k1, (void*)eqq.data(), x);
	a.write(k2, (void*)eqq.data(), x);
	a.write(k3, (void*)eqq.data(), x);
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